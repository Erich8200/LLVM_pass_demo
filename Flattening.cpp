//===- Flattening.cpp - Flattening Obfuscation pass------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the flattening pass
//
//===----------------------------------------------------------------------===//

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h> // For llvm::createLowerSwitchPass
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
#include <set>
#include "Flattening.h"
#include "CryptoUtils.h"
#include "Utils.h"

#define DEBUG_TYPE "flattening"

using namespace llvm;

// Stats
STATISTIC(Flattened, "Functions flattened");

namespace {
struct Flattening : public FunctionPass {
  static char ID;  // Pass identification, replacement for typeid
  bool flag;
  static llvm::Function* hx_func;
  
  Flattening() : FunctionPass(ID) {}
  Flattening(bool flag) : FunctionPass(ID) { this->flag = flag; }

  bool runOnFunction(Function &F);
  bool flatten(Function *f);

  void makeHashFunc(Function &F);

  unsigned int magic_prime = 0xC00005;
  unsigned int h(unsigned int x)
  {
    return x % magic_prime; 
  }
};
// }

// FunctionPass *lower;

char Flattening::ID = 0;
llvm::Function* Flattening::hx_func = nullptr;
static RegisterPass<Flattening> X("flattening", "Call graph flattening", 
                                  false, false);

static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                [](const llvm::PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) { 
                                    PM.add(createLowerSwitchPass());
                                    PM.add(new Flattening);
                                });

Pass *createFlattening(bool flag) { return new Flattening(flag); }

bool Flattening::runOnFunction(Function &F) {
  Function *tmp = &F;
  // Do we obfuscate
  // if (toObfuscate(flag, tmp, "fla")) {
  //   if (flatten(tmp)) {
  //     ++Flattened;
  //   }
  // }

  makeHashFunc(F);

  return flatten(tmp);
}

void Flattening::makeHashFunc(Function &F)
{
  if (!hx_func) {
    // Make <ret(arg)> type
    llvm::LLVMContext& context = F.getContext();
    llvm::Type* ret_type = llvm::Type::getInt32Ty(context);
    llvm::SmallVector<llvm::Type*, 2> func_arg_types;
    // func_arg_types.push_back(ret_type);
    func_arg_types.push_back(llvm::Type::getInt32Ty(context));
    llvm::FunctionType* func_type = 
                llvm::FunctionType::get(ret_type, func_arg_types, false);
    
    // Make function
    llvm::Function* func = llvm::Function::Create(
        func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage , "h", F.getParent());
    func->setCallingConv(llvm::CallingConv::C); // Set calling conversion, not strictly necessary
    
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", func);
    llvm::IRBuilder<> builder(bb);
    llvm::Function::arg_iterator argsIT = func->arg_begin(); // Make arg list
    llvm::Value* x = argsIT ++;
    llvm::Value* x_addr = builder.CreateAlloca(llvm::Type::getInt32Ty(context));
    builder.CreateStore(x, x_addr);
    llvm::Value* x_val = builder.CreateLoad(x_addr);
    llvm::Value* hx = builder.CreateURem(x_val, llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(context), magic_prime), "i9_12950hx");
    builder.CreateRet(hx);

    hx_func = func;
  }
}

bool Flattening::flatten(Function *f) {

  // Do not obfuscate hash function
  if (f == hx_func)
    return false;

  vector<BasicBlock *> origBB;
  BasicBlock *loopEntry;
  BasicBlock *loopEnd;
  LoadInst *load;
  SwitchInst *switchI;
  AllocaInst *switchVar;
  std::map<unsigned int, unsigned int> numCase_map;
  // std::set<unsigned int> num_set;

  // SCRAMBLER
  char scrambling_key[16];
  Utils::cryptoutils->get_bytes(scrambling_key, 16);
  // END OF SCRAMBLER

  // Lower switch
  // llvm::FunctionPassManager FPM;
  // FunctionPass *lower = createLowerSwitchPass();
  // FPM.addPass(lower);
  // lower->runOnFunction(*f);

  // Save all original BB
  for (Function::iterator i = f->begin(); i != f->end(); ++i) {
    BasicBlock *tmp = &*i;
    origBB.push_back(tmp);

    BasicBlock *bb = &*i;
    if (isa<InvokeInst>(bb->getTerminator())) {
      return false;
    }
  }

  // Nothing to flatten
  if (origBB.size() <= 1) {
    return false;
  }

  // Remove first BB
  origBB.erase(origBB.begin());

  // Get a pointer on the first BB
  Function::iterator tmp = f->begin();  //++tmp;
  BasicBlock *insert = &*tmp;

  // If main begin with an if
  BranchInst *br = NULL;
  if (isa<BranchInst>(insert->getTerminator())) {
    br = cast<BranchInst>(insert->getTerminator());
  }

  if ((br != NULL && br->isConditional()) ||
      insert->getTerminator()->getNumSuccessors() > 1) {
    BasicBlock::iterator i = insert->end();
	--i;

    if (insert->size() > 1) {
      --i;
    }

    BasicBlock *tmpBB = insert->splitBasicBlock(i, "first");
    origBB.insert(origBB.begin(), tmpBB);
  }

  // Remove jump
  insert->getTerminator()->eraseFromParent();

  // Create switch variable and set as it
  switchVar =
      new AllocaInst(Type::getInt32Ty(f->getContext()), 0, "switchVar", insert);
  // unsigned int x0 = Utils::cryptoutils->scramble32(0, scrambling_key);
  // unsigned int hx0 = h(x0); // This is very important! Or this value will not be updated
  new StoreInst(
      ConstantInt::get(Type::getInt32Ty(f->getContext()),
                       Utils::cryptoutils->scramble32(0, scrambling_key)),
      switchVar, insert);

  // Create main loop
  loopEntry = BasicBlock::Create(f->getContext(), "loopEntry", f, insert);
  loopEnd = BasicBlock::Create(f->getContext(), "loopEnd", f, insert);

  load = new LoadInst(Type::getInt32Ty(f->getContext()), switchVar, "switchVar", loopEntry);

  // Move first BB on top
  insert->moveBefore(loopEntry);
  BranchInst::Create(loopEntry, insert);

  // loopEnd jump to loopEntry
  BranchInst::Create(loopEntry, loopEnd);

  BasicBlock *swDefault = 
      BasicBlock::Create(f->getContext(), "switchDefault", f, loopEnd);
  BranchInst::Create(loopEnd, swDefault);

  // Create switch instruction itself and set condition
  switchI = SwitchInst::Create(&*f->begin(), swDefault, 0, loopEntry);
  switchI->setCondition(load);

  // Remove branch jump from 1st BB and make a jump to the while
  f->begin()->getTerminator()->eraseFromParent();

  BranchInst::Create(loopEntry, &*f->begin());

  // Put all BB in the switch
  for (vector<BasicBlock *>::iterator b = origBB.begin(); b != origBB.end();
       ++b) {
    BasicBlock *i = *b;
    ConstantInt *numCase = NULL;

    // Move the BB inside the switch (only visual, no code logic)
    i->moveBefore(loopEnd);

    // Add case to switch
    // numCase = cast<ConstantInt>(ConstantInt::get(
    //     switchI->getCondition()->getType(),
    //     Utils::cryptoutils->scramble32(switchI->getNumCases(), scrambling_key)));
    
    
    // numCase = h(x), carefully deal with collision
    if (switchI->getNumCases() > 0) { // Do not hash begin block's target switchVar
      // Can add some randomness here later
      unsigned int x = Utils::cryptoutils->scramble32(switchI->getNumCases(), scrambling_key);
      unsigned int hx = h(x);
      while (numCase_map.find(hx) != numCase_map.end()) {
        hx = h(++x);
      }

      // numCase = cast<ConstantInt>(ConstantInt::get(llvm::Type::getInt32Ty(i->getContext()), hx));
      numCase = cast<ConstantInt>(ConstantInt::get(switchI->getCondition()->getType(), hx));
      numCase_map[hx] = x;
    } else {
      numCase = cast<ConstantInt>(ConstantInt::get(switchI->getCondition()->getType(), Utils::cryptoutils->scramble32(0, scrambling_key)));
    }
    
    switchI->addCase(numCase, i);
  }

  // Recalculate switchVar
  for (vector<BasicBlock *>::iterator b = origBB.begin(); b != origBB.end();
       ++b) {
    BasicBlock *i = *b;

    // Ret BB
    if (i->getTerminator()->getNumSuccessors() == 0) {
      continue;
    }

    // If it's a non-conditional jump
    if (i->getTerminator()->getNumSuccessors() == 1) {
      ConstantInt *numCase = NULL;
      // Get successor and delete terminator
      BasicBlock *succ = i->getTerminator()->getSuccessor(0);
      i->getTerminator()->eraseFromParent();

      // I like IRBuilders!
      llvm::IRBuilder<> builder(i);

      // Get next case
      numCase = switchI->findCaseDest(succ);

      // If next case == default case (switchDefault)
      if (numCase == NULL) {
        numCase = cast<ConstantInt>(
            ConstantInt::get(switchI->getCondition()->getType(),
                             Utils::cryptoutils->scramble32(
                                 switchI->getNumCases() - 1, scrambling_key)));
      }

      // Update switchVar and jump to the end of loop
      // llvm::Value* hx = builder.CreateURem(llvm::ConstantInt::get(
      //                               llvm::Type::getInt32Ty(builder.getContext()), 
      //                               numCase_map[numCase]), 
      //                     llvm::ConstantInt::get(
      //                               llvm::Type::getInt32Ty(builder.getContext()), 
      //                               magic_prime));
      
      
      std::vector<llvm::Value*> args;
      uint32_t num = *(numCase->getValue().getRawData());
      if (numCase_map.find(num) != numCase_map.end()) {
        args.push_back(llvm::ConstantInt::get(
                              llvm::Type::getInt32Ty(builder.getContext()), 
                              numCase_map[num]
                              )
              );
        llvm::ArrayRef<llvm::Value *> args_ref(args);
        llvm::Value* hx = builder.CreateCall(llvm::FunctionCallee(hx_func), args_ref);
        builder.CreateStore(hx, load->getPointerOperand());
      } else {
        new StoreInst(numCase, load->getPointerOperand(), i);
      }
      
      // Debug print
      // llvm::ConstantInt* curNumCase = switchI->findCaseDest(i);
      // printf("%lx->%lx\n", *curNumCase->getValue().getRawData(), *numCase->getValue().getRawData());
      
      // new StoreInst(numCase, load->getPointerOperand(), i);
      BranchInst::Create(loopEnd, i);
      continue;
    }

    // If it's a conditional jump
    if (i->getTerminator()->getNumSuccessors() == 2) {
      // Get next cases
      ConstantInt *numCaseTrue =
          switchI->findCaseDest(i->getTerminator()->getSuccessor(0));
      ConstantInt *numCaseFalse =
          switchI->findCaseDest(i->getTerminator()->getSuccessor(1));

      // Check if next case == default case (switchDefault)
      if (numCaseTrue == NULL) {
        numCaseTrue = cast<ConstantInt>(
            ConstantInt::get(switchI->getCondition()->getType(),
                             Utils::cryptoutils->scramble32(
                                 switchI->getNumCases() - 1, scrambling_key)));
      }

      if (numCaseFalse == NULL) {
        numCaseFalse = cast<ConstantInt>(
            ConstantInt::get(switchI->getCondition()->getType(),
                             Utils::cryptoutils->scramble32(
                                 switchI->getNumCases() - 1, scrambling_key)));
      }

      SelectInst *sel = nullptr;
      std::vector<llvm::Value*> args_t, args_f;
      llvm::IRBuilder<> builder(i->getTerminator());
      uint32_t num_true = *(numCaseTrue->getValue().getRawData());
      uint32_t num_false = *(numCaseFalse->getValue().getRawData());
      BranchInst *br = cast<BranchInst>(i->getTerminator());

      if (numCase_map.find(num_true) != numCase_map.end() && 
          numCase_map.find(num_false) != numCase_map.end()) {
        args_t.push_back(llvm::ConstantInt::get(
                      llvm::Type::getInt32Ty(builder.getContext()), 
                      numCase_map[num_true]));
        args_f.push_back(llvm::ConstantInt::get(
                      llvm::Type::getInt32Ty(builder.getContext()), 
                      numCase_map[num_false]));
        llvm::ArrayRef<llvm::Value *> args_ref_t(args_t);
        llvm::ArrayRef<llvm::Value *> args_ref_f(args_f);
        llvm::Value* hx_t = builder.CreateCall(llvm::FunctionCallee(hx_func), args_ref_t);
        llvm::Value* hx_f = builder.CreateCall(llvm::FunctionCallee(hx_func), args_ref_f);
        // builder.CreateStore(hx, load->getPointerOperand());
        sel = llvm::cast<llvm::SelectInst>(builder.CreateSelect(br->getCondition(), hx_t, hx_f));
      } else {
        // Create a SelectInst
        sel = SelectInst::Create(br->getCondition(), numCaseTrue, numCaseFalse, "",
                            i->getTerminator());
      }

      // Debug print
      // llvm::ConstantInt* curNumCase = switchI->findCaseDest(i);
      // printf("%lx->%lx/%lx\n", *curNumCase->getValue().getRawData(), 
      //                         *numCaseTrue->getValue().getRawData(), 
      //                         *numCaseFalse->getValue().getRawData());

      // Erase terminator
      i->getTerminator()->eraseFromParent();

      // Update switchVar and jump to the end of loop
      new StoreInst(sel, load->getPointerOperand(), i);
      BranchInst::Create(loopEnd, i);
      continue;
    }
  }

  fixStack(f);

  return true;
}

}
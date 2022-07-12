//===- BogusControlFlow.cpp - BogusControlFlow Obfuscation pass from ollvm-------------===//
//===----------------------------------------------------------------------------------===//

#include "BogusControlFlow.h"
// #include "Utils.h"
#include <iostream>
#include <set>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <sstream>
#include <string>
#include <llvm/IR/Dominators.h>
#include <memory>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

// Options for the pass
const int defaultObfRate = 30, defaultObfTime = 1;

static cl::opt<int>
ObfProbRate("bcf_prob", cl::desc("Choose the probability [%] each basic blocks will be obfuscated by the -bcf pass"), cl::value_desc("probability rate"), cl::init(defaultObfRate), cl::Optional);

static cl::opt<int>
ObfTimes("bcf_loop", cl::desc("Choose how many time the -bcf pass loop on a function"), cl::value_desc("number of times"), cl::init(defaultObfTime), cl::Optional);

namespace {
  struct BogusControlFlow : public FunctionPass {
    static char ID; // Pass identification
    bool flag;

    BogusControlFlow() : FunctionPass(ID) {}
    BogusControlFlow(bool flag) : FunctionPass(ID) {this->flag = flag; BogusControlFlow();}

    /* runOnFunction
     *
     * Overwrite FunctionPass method to apply the transformation
     * to the function. See header for more details.
     */
    virtual bool runOnFunction(Function &F){
      // Designate obf params
      ObfTimes = 1;
      ObfProbRate = 50;

      // Check if the percentage is correct
      if (ObfTimes <= 0) {
        errs()<<"BogusControlFlow application number -bcf_loop=x must be x > 0";
	    	return false;
      }

      // Check if the number of applications is correct
      if ( !((ObfProbRate > 0) && (ObfProbRate <= 100)) ) {
        errs()<<"BogusControlFlow application basic blocks percentage -bcf_prob=x must be 0 < x <= 100";
	    	return false;
      }
      // If fla annotations
      // if(toObfuscate(flag,&F,"bcf")) {
      //   bogus(F);
      //   doF(*F.getParent());
      //   return true;
      // }

      if (F.getName() == "h") {
        return false;
      }
      

      bogus(F);
      doF(*F.getParent());
      return true;

    } // end of runOnFunction()

    void bogus(Function &F) {
      // For statistics and debug
      int NumBasicBlocks = 0;
      bool firstTime = true; // First time we do the loop in this function
      bool hasBeenModified = false;
      DEBUG_WITH_TYPE("opt", errs() << "bcf: Started on function " << F.getName() << "\n");
      DEBUG_WITH_TYPE("opt", errs() << "bcf: Probability rate: "<< ObfProbRate<< "\n");
      if(ObfProbRate < 0 || ObfProbRate > 100){
        DEBUG_WITH_TYPE("opt", errs() << "bcf: Incorrect value,"
            << " probability rate set to default value: "
            << defaultObfRate <<" \n");
        ObfProbRate = defaultObfRate;
      }
      DEBUG_WITH_TYPE("opt", errs() << "bcf: How many times: "<< ObfTimes<< "\n");
      if(ObfTimes <= 0){
        DEBUG_WITH_TYPE("opt", errs() << "bcf: Incorrect value,"
            << " must be greater than 1. Set to default: "
            << defaultObfTime <<" \n");
        ObfTimes = defaultObfTime;
      }

      int NumObfTimes = ObfTimes;

        // Real begining of the pass
        // Loop for the number of time we run the pass on the function
        do{
          DEBUG_WITH_TYPE("cfg", errs() << "bcf: Function " << F.getName()
              <<", before the pass:\n");
          DEBUG_WITH_TYPE("cfg", F.viewCFG());
          // Put all the function's block in a list
          std::list<BasicBlock *> basicBlocks;
          for (Function::iterator i=F.begin();i!=F.end();++i) {
            basicBlocks.push_back(&*i);
          }
          DEBUG_WITH_TYPE("gen", errs() << "bcf: Iterating on the Function's Basic Blocks\n");

          while(!basicBlocks.empty()){
            NumBasicBlocks ++;
            // Basic Blocks' selection
            if((int)Utils::cryptoutils->get_range(100) <= ObfProbRate){
              DEBUG_WITH_TYPE("opt", errs() << "bcf: Block "
                  << NumBasicBlocks <<" selected. \n");
              hasBeenModified = true;
              // ++NumModifiedBasicBlocks;
              // NumAddedBasicBlocks += 3;
              // FinalNumBasicBlocks += 3;
              // Add bogus flow to the given Basic Block (see description)
              BasicBlock *basicBlock = basicBlocks.front();
              addBogusFlow(basicBlock, F);
            }
            else{
              DEBUG_WITH_TYPE("opt", errs() << "bcf: Block "
                  << NumBasicBlocks <<" not selected.\n");
            }
            // remove the block from the list
            basicBlocks.pop_front();

            if(firstTime){ // first time we iterate on this function
              // ++InitNumBasicBlocks;
              // ++FinalNumBasicBlocks;
            }
          } // end of while(!basicBlocks.empty())
          DEBUG_WITH_TYPE("gen", errs() << "bcf: End of function " << F.getName() << "\n");
          if(hasBeenModified){ // if the function has been modified
            DEBUG_WITH_TYPE("cfg", errs() << "bcf: Function " << F.getName()
                <<", after the pass: \n");
            DEBUG_WITH_TYPE("cfg", F.viewCFG());
          }
          else{
            DEBUG_WITH_TYPE("cfg", errs() << "bcf: Function's not been modified \n");
          }
          firstTime = false;
        }while(--NumObfTimes > 0);
    }

    /* addBogusFlow
     *
     * Add bogus flow to a given basic block, according to the header's description
     */
    virtual void addBogusFlow(BasicBlock * basicBlock, Function &F){


      // Split the block: first part with only the phi nodes and debug info and terminator
      //                  created by splitBasicBlock. (-> No instruction)
      //                  Second part with every instructions from the original block
      // We do this way, so we don't have to adjust all the phi nodes, metadatas and so on
      // for the first block. We have to let the phi nodes in the first part, because they
      // actually are updated in the second part according to them.
      BasicBlock::iterator i1 = basicBlock->begin();
      if(basicBlock->getFirstNonPHIOrDbgOrLifetime())
        i1 = (BasicBlock::iterator)basicBlock->getFirstNonPHIOrDbgOrLifetime();
      Twine *var;
      var = new Twine("originalBB");
      BasicBlock *originalBB = basicBlock->splitBasicBlock(i1, *var);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: First and original basic blocks: ok\n");

      // Creating the altered basic block on which the first basicBlock will jump
      Twine * var3 = new Twine("alteredBB");
      BasicBlock *alteredBB = createAlteredBasicBlock(originalBB, *var3, &F);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Altered basic block: ok\n");

      // Now that all the blocks are created,
      // we modify the terminators to adjust the control flow.

      alteredBB->getTerminator()->eraseFromParent();
      basicBlock->getTerminator()->eraseFromParent();
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Terminator removed from the altered"
          <<" and first basic blocks\n");

      // Preparing a condition..
      // For now, the condition is an always true comparaison between 2 float
      // This will be complicated after the pass (in doFinalization())
      Value * LHS = ConstantFP::get(Type::getFloatTy(F.getContext()), 1.0);
      Value * RHS = ConstantFP::get(Type::getFloatTy(F.getContext()), 1.0);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Value LHS and RHS created\n");


      // The always true condition. End of the first block
      Twine * var4 = new Twine("condition");
      FCmpInst * condition = new FCmpInst(*basicBlock, FCmpInst::FCMP_TRUE , LHS, RHS, *var4);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Always true condition created\n");

      // Jump to the original basic block if the condition is true or
      // to the altered block if false.
      BranchInst::Create(originalBB, alteredBB, (Value *)condition, basicBlock);
      DEBUG_WITH_TYPE("gen",
          errs() << "bcf: Terminator instruction in first basic block: ok\n");

      // The altered block loop back on the original one.
      BranchInst::Create(originalBB, alteredBB);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Terminator instruction in altered block: ok\n");


      // The end of the originalBB is modified to give the impression that sometimes
      // it continues in the loop, and sometimes it return the desired value
      // (of course it's always true, so it always use the original terminator..
      //  but this will be obfuscated too;) )

      // iterate on instruction just before the terminator of the originalBB
      BasicBlock::iterator i = originalBB->end();

      // Split at this point (we only want the terminator in the second part)
      Twine * var5 = new Twine("originalBBpart2");
      BasicBlock * originalBBpart2 = originalBB->splitBasicBlock(--i , *var5);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Terminator part of the original basic block"
          << " is isolated\n");
      // the first part go either on the return statement or on the begining
      // of the altered block.. So we erase the terminator created when splitting.
      originalBB->getTerminator()->eraseFromParent();
      // We add at the end a new always true condition
      Twine * var6 = new Twine("condition2");
      FCmpInst * condition2 = new FCmpInst(*originalBB, CmpInst::FCMP_TRUE , LHS, RHS, *var6);
      BranchInst::Create(originalBBpart2, alteredBB, (Value *)condition2, originalBB);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Terminator original basic block: ok\n");
      DEBUG_WITH_TYPE("gen", errs() << "bcf: End of addBogusFlow().\n");

    } // end of addBogusFlow()

    /* createAlteredBasicBlock
     *
     * This function return a basic block similar to a given one.
     * It's inserted just after the given basic block.
     * The instructions are similar but junk instructions are added between
     * the cloned one. The cloned instructions' phi nodes, metadatas, uses and
     * debug locations are adjusted to fit in the cloned basic block and
     * behave nicely.
     */
    virtual BasicBlock* createAlteredBasicBlock(BasicBlock * basicBlock,
        const Twine &  Name = "gen", Function * F = 0){
      // Useful to remap the informations concerning instructions.
      ValueToValueMapTy VMap;
      BasicBlock * alteredBB = llvm::CloneBasicBlock (basicBlock, VMap, Name, F);
      DEBUG_WITH_TYPE("gen", errs() << "bcf: Original basic block cloned\n");
      // Remap operands.
      BasicBlock::iterator ji = basicBlock->begin();
      for (BasicBlock::iterator i = alteredBB->begin(), e = alteredBB->end() ; i != e; ++i){
        // Loop over the operands of the instruction
        for(User::op_iterator opi = i->op_begin (), ope = i->op_end(); opi != ope; ++opi){
          // get the value for the operand
          Value *v = MapValue(*opi, VMap,  RF_None, 0);
          if (v != 0){
            *opi = v;
            DEBUG_WITH_TYPE("gen", errs() << "bcf: Value's operand has been setted\n");
          }
        }
        DEBUG_WITH_TYPE("gen", errs() << "bcf: Operands remapped\n");
        // Remap phi nodes' incoming blocks.
        if (PHINode *pn = dyn_cast<PHINode>(i)) {
          for (unsigned j = 0, e = pn->getNumIncomingValues(); j != e; ++j) {
            Value *v = MapValue(pn->getIncomingBlock(j), VMap, RF_None, 0);
            if (v != 0){
              pn->setIncomingBlock(j, cast<BasicBlock>(v));
            }
          }
        }
        DEBUG_WITH_TYPE("gen", errs() << "bcf: PHINodes remapped\n");
        // Remap attached metadata.
        SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
        i->getAllMetadata(MDs);
        DEBUG_WITH_TYPE("gen", errs() << "bcf: Metadatas remapped\n");
        // important for compiling with DWARF, using option -g.
        i->setDebugLoc(ji->getDebugLoc());
        ji++;
        DEBUG_WITH_TYPE("gen", errs() << "bcf: Debug information location setted\n");

      } // The instructions' informations are now all correct

      DEBUG_WITH_TYPE("gen", errs() << "bcf: The cloned basic block is now correct\n");
      DEBUG_WITH_TYPE("gen",
          errs() << "bcf: Starting to add junk code in the cloned bloc...\n");

      // add random instruction in the middle of the bloc. This part can be improve
      for (BasicBlock::iterator i = alteredBB->begin(), e = alteredBB->end() ; i != e; ++i){
        // in the case we find binary operator, we modify slightly this part by randomly
        // insert some instructions
        if(i->isBinaryOp()){ // binary instructions
          unsigned opcode = i->getOpcode();
          BinaryOperator *op, *op1 = NULL;
          Twine *var = new Twine("_");
          // treat differently float or int
          // Binary int
          if(opcode == Instruction::Add || opcode == Instruction::Sub ||
              opcode == Instruction::Mul || opcode == Instruction::UDiv ||
              opcode == Instruction::SDiv || opcode == Instruction::URem ||
              opcode == Instruction::SRem || opcode == Instruction::Shl ||
              opcode == Instruction::LShr || opcode == Instruction::AShr ||
              opcode == Instruction::And || opcode == Instruction::Or ||
              opcode == Instruction::Xor){
            for(int random = (int)Utils::cryptoutils->get_range(10); random < 10; ++random){
              // switch(Utils::cryptoutils->get_range(4)){ // to improve
              switch(Utils::cryptoutils->get_range(3)){ // to improve
                // case 0: //do nothing
                //   break;
                case 0: op = BinaryOperator::CreateNeg(i->getOperand(0),*var,&*i);
                        op1 = BinaryOperator::Create(Instruction::Add,op,
                            i->getOperand(1),"gen",&*i);
                        break;
                case 1: op1 = BinaryOperator::Create(Instruction::Sub,
                            i->getOperand(0),
                            i->getOperand(1),*var,&*i);
                        op = BinaryOperator::Create(Instruction::Mul,op1,
                            i->getOperand(1),"gen",&*i);
                        break;
                case 2: op = BinaryOperator::Create(Instruction::Shl,
                            i->getOperand(0),
                            i->getOperand(1),*var,&*i);
                        break;
              }
            }
          }
          // Binary float
          if(opcode == Instruction::FAdd || opcode == Instruction::FSub ||
              opcode == Instruction::FMul || opcode == Instruction::FDiv ||
              opcode == Instruction::FRem){
            for(int random = (int)Utils::cryptoutils->get_range(10); random < 10; ++random){
              // switch(Utils::cryptoutils->get_range(3)){ // can be improved
              switch(Utils::cryptoutils->get_range(2)){ // can be improved
                // case 0: //do nothing
                //   break;
                case 0: 
                        // op = BinaryOperator::CreateFNeg(i->getOperand(0),*var,&*i);
                        // op1 = BinaryOperator::Create(Instruction::FAdd,op,
                        //     i->getOperand(1),"gen",&*i);
                        // break;
                case 1: op = BinaryOperator::Create(Instruction::FSub,
                            i->getOperand(0),
                            i->getOperand(1),*var,&*i);
                        op1 = BinaryOperator::Create(Instruction::FMul,op,
                            i->getOperand(1),"gen",&*i);
                        break;
              }
            }
          }
          if(opcode == Instruction::ICmp){ // Condition (with int)
            ICmpInst *currentI = (ICmpInst*)(&i);
            // switch(Utils::cryptoutils->get_range(3)){ // must be improved
            switch(Utils::cryptoutils->get_range(2)){ // must be improved
              // case 0: //do nothing
              //   break;
              case 0: currentI->swapOperands();
                      break;
              case 1: // randomly change the predicate
                      switch(Utils::cryptoutils->get_range(10)){
                        case 0: currentI->setPredicate(ICmpInst::ICMP_EQ);
                                break; // equal
                        case 1: currentI->setPredicate(ICmpInst::ICMP_NE);
                                break; // not equal
                        case 2: currentI->setPredicate(ICmpInst::ICMP_UGT);
                                break; // unsigned greater than
                        case 3: currentI->setPredicate(ICmpInst::ICMP_UGE);
                                break; // unsigned greater or equal
                        case 4: currentI->setPredicate(ICmpInst::ICMP_ULT);
                                break; // unsigned less than
                        case 5: currentI->setPredicate(ICmpInst::ICMP_ULE);
                                break; // unsigned less or equal
                        case 6: currentI->setPredicate(ICmpInst::ICMP_SGT);
                                break; // signed greater than
                        case 7: currentI->setPredicate(ICmpInst::ICMP_SGE);
                                break; // signed greater or equal
                        case 8: currentI->setPredicate(ICmpInst::ICMP_SLT);
                                break; // signed less than
                        case 9: currentI->setPredicate(ICmpInst::ICMP_SLE);
                                break; // signed less or equal
                      }
                      break;
            }
          }
          if(opcode == Instruction::FCmp){ // Conditions (with float)
            FCmpInst *currentI = (FCmpInst*)(&i);
            // switch(Utils::cryptoutils->get_range(3)){ // must be improved
            switch(Utils::cryptoutils->get_range(2)){ // must be improved
              // case 0: //do nothing
              //   break;
              case 0: currentI->swapOperands();
                      break;
              case 1: // randomly change the predicate
                      switch(Utils::cryptoutils->get_range(10)){
                        case 0: currentI->setPredicate(FCmpInst::FCMP_OEQ);
                                break; // ordered and equal
                        case 1: currentI->setPredicate(FCmpInst::FCMP_ONE);
                                break; // ordered and operands are unequal
                        case 2: currentI->setPredicate(FCmpInst::FCMP_UGT);
                                break; // unordered or greater than
                        case 3: currentI->setPredicate(FCmpInst::FCMP_UGE);
                                break; // unordered, or greater than, or equal
                        case 4: currentI->setPredicate(FCmpInst::FCMP_ULT);
                                break; // unordered or less than
                        case 5: currentI->setPredicate(FCmpInst::FCMP_ULE);
                                break; // unordered, or less than, or equal
                        case 6: currentI->setPredicate(FCmpInst::FCMP_OGT);
                                break; // ordered and greater than
                        case 7: currentI->setPredicate(FCmpInst::FCMP_OGE);
                                break; // ordered and greater than or equal
                        case 8: currentI->setPredicate(FCmpInst::FCMP_OLT);
                                break; // ordered and less than
                        case 9: currentI->setPredicate(FCmpInst::FCMP_OLE);
                                break; // ordered or less than, or equal
                      }
                      break;
            }
          }
        }
      }
      return alteredBB;
    } // end of createAlteredBasicBlock()


    /* doFinalization
     *
     * Overwrite FunctionPass method to apply the transformations to the whole module.
     * This part obfuscate all the always true predicates of the module.
     * More precisely, the condition which predicate is FCMP_TRUE.
     * It also remove all the functions' basic blocks' and instructions' names.
     */
    bool doF(Module &M){
      // In this part we extract all always-true predicate and replace them with opaque predicate:
      // For this, we declare two global values: x and y, and replace the FCMP_TRUE predicate with
      // (y < 10 || x * (x + 1) % 2 == 0)
      // A better way to obfuscate the predicates would be welcome.
      // In the meantime we will erase the name of the basic blocks, the instructions
      // and the functions.

      std::vector<Instruction*> toEdit, toDelete;

      // Looking for the conditions and branches to transform
      for(Module::iterator mi = M.begin(), me = M.end(); mi != me; ++mi){
        for(Function::iterator fi = mi->begin(), fe = mi->end(); fi != fe; ++fi){
          // TerminatorInst * tbb = fi->getTerminator();
          llvm::Instruction* tbb = fi->getTerminator();
          if(tbb->getOpcode() == Instruction::Br){
            BranchInst * br = (BranchInst *)(tbb);
            if(br->isConditional()){
              FCmpInst * cond = (FCmpInst *)br->getCondition();
              unsigned opcode = cond->getOpcode();
              if(opcode == Instruction::FCmp){
                if (cond->getPredicate() == FCmpInst::FCMP_TRUE){
                  toDelete.push_back(cond); // The condition
                  toEdit.push_back(tbb);    // The branch using the condition
                }
              }
            }
          }
        }
      }

      // Replacing all the branches we found
      for(std::vector<Instruction*>::iterator i =toEdit.begin();i!=toEdit.end();++i) {
       
        llvm::BasicBlock* cur_bb = (*i)->getParent();
        llvm::LLVMContext& context = cur_bb->getContext();

        // Try to find values
        // while (cur_bb && !got_two) {
        //   for (llvm::Instruction& I : *cur_bb) {
        //       if (llvm::isa<llvm::LoadInst>(&I) || llvm::isa<llvm::BinaryOperator>(&I)) {
        //           if (
        //               // I.getType() == llvm::Type::getInt1Ty(context) || 
        //               // I.getType() == llvm::Type::getInt8Ty(context) || 
        //               // I.getType() == llvm::Type::getInt16Ty(context) ||
        //               I.getType() == llvm::Type::getInt32Ty(context)
        //               // I.getType() == llvm::Type::getInt64Ty(context)
        //               // I.getType() == llvm::Type::getHalfTy(context) ||
        //               // I.getType() == llvm::Type::getBFloatTy(context) ||
        //               // I.getType() == llvm::Type::getFloatTy(context) ||
        //               // I.getType() == llvm::Type::getDoubleTy(context)
        //               ) {
        //                 if (!got_one) {
        //                   x = &I;
        //                   got_one = true;
        //                 } else {
        //                   y = &I;
        //                   got_two = true;
        //                   break;
        //                 }
        //           }
        //       }
        //   }
        //   cur_bb = cur_bb->getUniquePredecessor();
        // }

        // Try to find 2 int Values, x and y
        llvm::Type* int32_type = llvm::Type::getInt32Ty(context);
        std::set<Value*> vals;
        llvm::DominatorTree* domInfo = new llvm::DominatorTree(*(cur_bb->getParent()));
        findLocalVals(cur_bb, int32_type, 2, vals, domInfo);
        std::vector<llvm::Value*> vals_arr; // Transform set to vector
        for (auto it = vals.begin(); it != vals.end(); it++) {
          vals_arr.push_back(*it);
        }

        llvm::BasicBlock *t_BB = ((BranchInst*)*i)->getSuccessor(0), 
                         *f_BB = ((BranchInst*)*i)->getSuccessor(1);
        llvm::IRBuilder<> builder(*i);

        // Randomly create opaqe predicate
        switch(Utils::cryptoutils->get_range(5)){
          case 0: //if y < 10 || x*(x+1) % 2 == 0
          {
            BinaryOperator *op,*op1 = NULL;
            ICmpInst * condition, * condition2;
            llvm::Value* x = vals_arr[0];
            llvm::Value* y = vals_arr[1];

            op = BinaryOperator::Create(Instruction::Add, (Value *)x,
                ConstantInt::get(Type::getInt32Ty(M.getContext()), 1,
                  false), "", (*i));
            op1 = BinaryOperator::Create(Instruction::Mul, (Value *)x, op, "", (*i));
            op = BinaryOperator::Create(Instruction::URem, op1,
                ConstantInt::get(Type::getInt32Ty(M.getContext()), 2,
                  false), "", (*i));

            condition = new ICmpInst((*i), ICmpInst::ICMP_EQ, op,
                ConstantInt::get(Type::getInt32Ty(M.getContext()), 0,
                  false));

            condition2 = new ICmpInst((*i), ICmpInst::ICMP_SLT, y,
                ConstantInt::get(Type::getInt32Ty(M.getContext()), 10,
                  false));
            
            op1 = BinaryOperator::Create(Instruction::Or, (Value *)condition,
                (Value *)condition2, "", (*i));
          
            BranchInst::Create(t_BB, f_BB, (Value *) op1, ((BranchInst*)*i)->getParent());
            
            break;
          }
          case 1:  // x^2 - 34y^2 != -1 | x,y is Z
          {
            llvm::Value* x = vals_arr[0];
            llvm::Value* y = vals_arr[1];
            llvm::Value* mul1 = builder.CreateMul(x, x, "mul1");
            llvm::Value* mul2 = builder.CreateMul(
                                      builder.CreateMul(y, y, "mul2_1"), 
                                      llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 34), 
                                      "mul2"
                                  );
            llvm::Value* sub = builder.CreateSub(mul1, mul2, "sub");
            llvm::Value* cmp = builder.CreateICmpNE(sub, 
                                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 
                                -1, "cmp"));
            builder.CreateCondBr(cmp, t_BB, f_BB);
            break;
          }
          case 2:  // 7y^2 - x^2 != 1 | x,y is Z
          {
            llvm::Value* x = vals_arr[0];
            llvm::Value* y = vals_arr[1];
            llvm::Value* mul1 = builder.CreateMul(x, x, "mul1");
            llvm::Value* mul2 = builder.CreateMul(
                                      builder.CreateMul(y, y, "mul2_1"), 
                                      llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 7), 
                                      "mul2"
                                  );
            llvm::Value* sub = builder.CreateSub(mul2, mul1, "sub");
            llvm::Value* cmp = builder.CreateICmpNE(sub, 
                                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 
                                1, "cmp"));
            builder.CreateCondBr(cmp, t_BB, f_BB);
            break;
          }
          case 3: // 4ab <= (a + b)^2
          {
            llvm::Value* x = vals_arr[0];
            llvm::Value* y = vals_arr[1];
            llvm::Value* a = builder.CreateSIToFP(x, llvm::Type::getDoubleTy(context));
            llvm::Value* b = builder.CreateSIToFP(y, llvm::Type::getDoubleTy(context));
            llvm::Value* _4ab = builder.CreateFMul(
                                        ConstantFP::get(Type::getDoubleTy(context), 4.0), 
                                        builder.CreateFMul(a, b), "_4ab"
                                        );
            llvm::Value* a2 = builder.CreateFMul(a, a, "a2");
            llvm::Value* b2 = builder.CreateFMul(b, b, "b2");
            llvm::Value* a2_add_b2 = builder.CreateFAdd(a2, b2, "a2_add_b2");
            llvm::Value* a2_add_b2_2 = builder.CreateFMul(a2_add_b2, a2_add_b2, "a2_add_b2_2");
            llvm::Value* cmp = builder.CreateFCmpOGE(a2_add_b2_2, _4ab, "cmp");
            builder.CreateCondBr(cmp, t_BB, f_BB);
            break;
          }
          case 4: // (a + b)^2 <= 2(a^2 + b^2)
          {
            llvm::Value* x = vals_arr[0];
            llvm::Value* y = vals_arr[1];
            llvm::Value* a = builder.CreateSIToFP(x, llvm::Type::getDoubleTy(context));
            llvm::Value* b = builder.CreateSIToFP(y, llvm::Type::getDoubleTy(context));
            llvm::Value* a2 = builder.CreateFMul(a, a, "a2");
            llvm::Value* b2 = builder.CreateFMul(b, b, "b2");
            llvm::Value* a_add_b_2 = builder.CreateFAdd(
                b2, 
                  builder.CreateFAdd(
                    builder.CreateFMul(
                                        ConstantFP::get(Type::getDoubleTy(context), 2.0), 
                                        builder.CreateFMul(a, b),
                                        "2ab"
                                      ), a2)
            );

            llvm::Value* _2_a2_add_b2 = builder.CreateFMul(
              builder.CreateFAdd(a2, b2), 
              ConstantFP::get(Type::getDoubleTy(context), 2.0), 
              "_2_a2_add_b2"
            );
            llvm::Value* cmp = builder.CreateFCmpOLE(a_add_b_2, _2_a2_add_b2, "cmp");
            builder.CreateCondBr(cmp, t_BB, f_BB);
            break;
          }
          // case 5: { // (a^2 + b^2)(c^2 + d^2) >= (ac + bd)^2 | a,b,c,d is R
          //   llvm::Value* a = vals_arr[0];
          //   llvm::Value* b = vals_arr[1];
          //   llvm::Value* c = vals_arr[2];
          //   llvm::Value* d = vals_arr[3];
          //   llvm::Value* sqra = builder.CreateMul(a, a, "sqra");
          //   llvm::Value* sqrb = builder.CreateMul(b, b, "sqrb");
          //   llvm::Value* sqrc = builder.CreateMul(c, c, "sqrc");
          //   llvm::Value* sqrd = builder.CreateMul(d, d, "sqrd");

          //   llvm::Value* ac = builder.CreateMul(a, c, "ac");
          //   llvm::Value* bd = builder.CreateMul(b, d, "bd");

          //   llvm::Value* add1 = builder.CreateSub(sqra, sqrb, "add1");
          //   llvm::Value* add2 = builder.CreateSub(sqrc, sqrd, "add2");

          //   llvm::Value* mul1 = builder.CreateMul(add1, add2, "mul1");

          //   llvm::Value* add3 = builder.CreateSub(ac, bd, "add3");

          //   llvm::Value* mul2 = builder.CreateMul(add3, add3, "mul2");

          //   llvm::Value* cmp = builder.CreateICmpNE(mul1, mul2, "cmp");
          //   builder.CreateCondBr(cmp, t_BB, f_BB);
          //   break;
          // }
          default:
            break;
        }

        (*i)->eraseFromParent(); // erase the branch        
      }
    
      // Erase all the associated conditions we found
      for(std::vector<Instruction*>::iterator i =toDelete.begin();i!=toDelete.end();++i){
        (*i)->eraseFromParent();
      }

      return true;
    
    }

    // Find @num dominating def Values, if @num le 0, the function will find all
    void findLocalValsInSuccessors(llvm::BasicBlock* cur_BB, 
                                    const llvm::Type* T, const int num, 
                                    std::set<llvm::Value*>& vals, 
                                    const llvm::DominatorTree* DomInfo, 
                                    const llvm::BasicBlock* start_BB, 
                                    std::set<llvm::Value*>& visited)
    {
        if (num > 0 && vals.size() >= num)
            return;
        
        if (visited.find(cur_BB) != visited.end())
            return;
        else
            visited.insert(cur_BB);
        
        for (llvm::BasicBlock* BB : llvm::predecessors(cur_BB)) {
            for (llvm::Instruction& I : *BB) {
                if (I.getType() == T) {
                    if ((llvm::isa<llvm::LoadInst>(&I) || llvm::isa<llvm::BinaryOperator>(&I))) {
                        if (DomInfo->dominates(&I, start_BB)) {
                            vals.insert(&I);
                        }
                    } else if (llvm::isa<llvm::StoreInst>(&I)) {
                        llvm::Instruction* target_ins = 
                                            llvm::dyn_cast<llvm::Instruction>(I.getOperand(1));
                        if (target_ins && DomInfo->dominates(target_ins, start_BB)) {
                            vals.insert(target_ins);
                        }
                    }
                }
            }

            findLocalValsInSuccessors(BB, T, num, vals, DomInfo, start_BB, visited);    
        }
        
    }

    void findLocalVals(llvm::BasicBlock* cur_BB, 
                        llvm::Type* T, int num, std::set<llvm::Value*>& vals, 
                        llvm::DominatorTree* domInfo)
    {
      // Find in cur block firstly
      for (llvm::Instruction& I : *cur_BB) {
          if (I.getType() == T) {
              if ((llvm::isa<llvm::LoadInst>(&I) || llvm::isa<llvm::BinaryOperator>(&I))) {
                vals.insert(&I);
              } else if (llvm::isa<llvm::StoreInst>(&I)) {
                vals.insert(I.getOperand(1));
              }
          }
      }

      if (num > 0 && vals.size() >= num)
        return;

      std::set<llvm::Value*> visited;
      findLocalValsInSuccessors(cur_BB, T, 2, vals, domInfo, cur_BB, visited);

      // Find in entry block secondly
      // if (vals.size() < num) {
      //   for (llvm::Instruction& I : entry_BB) {
      //     if (I.getType() == T) {
      //         if ((llvm::isa<llvm::LoadInst>(&I) || llvm::isa<llvm::BinaryOperator>(&I))) {
      //           vals.push_back(&I);
      //         } else if (llvm::isa<llvm::StoreInst>(&I)) {
      //           vals.push_back(I.getOperand(1));
      //         }
      //     }
      //   }
      // }

      if (num > 0 && vals.size() >= num)
        return;

      // Create manually if the second aproach still fails
      llvm::BasicBlock& entry_BB = cur_BB->getParent()->getEntryBlock();
      llvm::Instruction* terminator_ins = entry_BB.getTerminator();
      llvm::IRBuilder<> builder(terminator_ins);
      int e = num - vals.size();
      for (int i = 0; i < e; i++) {
        llvm::AllocaInst* alloc_ins = builder.CreateAlloca(T);
        builder.CreateStore(llvm::ConstantInt::get(T, Utils::cryptoutils->get_uint32_t()), 
                                                    alloc_ins);
        std::stringstream ss;
        ss << vals.size();
        std::string name;
        ss >> name;
        llvm::LoadInst* load_ins = builder.CreateLoad(alloc_ins, "var_" + name);

        // std::stringstream ss;
        // ss << vals.size();
        // std::string name;
        // ss >> name;

        // llvm::AllocaInst* alloc_ins = new llvm::AllocaInst(T, 0, "", terminator_ins);
        // new llvm::StoreInst(llvm::ConstantInt::get(T, 666), alloc_ins, terminator_ins);
        // llvm::LoadInst* load_ins = new llvm::LoadInst(T, alloc_ins, "var_" + name, terminator_ins);

        vals.insert(load_ins);
      }
    }

  };
}

char BogusControlFlow::ID = 0;
static RegisterPass<BogusControlFlow> X("boguscf", "Inserting bogus control flow");

static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                [](const llvm::PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) { 
                                    PM.add(new BogusControlFlow);
                                });

Pass *llvm::createBogus() {
  return new BogusControlFlow();
}

Pass *llvm::createBogus(bool flag) {
  return new BogusControlFlow(flag);
}

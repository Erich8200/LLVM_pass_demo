#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/User.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <string>
#include <iostream>
#include <vector>

namespace {

    struct InsertBB : public llvm::FunctionPass {
        static char ID;

        InsertBB() : FunctionPass(ID) { }

        virtual bool runOnFunction(llvm::Function& F) override {         
            
            std::vector<llvm::BasicBlock*> basic_blocks;

            for (llvm::BasicBlock& B : F)
                basic_blocks.push_back(&B);

            for (auto b = basic_blocks.begin(); b < basic_blocks.end(); ++b) {
                
                // Get LLVMContext
                llvm::LLVMContext& context = (*b)->getContext();

                // Create alloc-store-load ins
                llvm::BasicBlock* orig_suc_block = (*b)->getUniqueSuccessor();
                if (!orig_suc_block)
                {
                    continue;
                }
                
                llvm::BasicBlock* insert_BB = llvm::BasicBlock::Create(context, 
                                                "InsertBB", 
                                                (*b)->getParent(), 
                                                orig_suc_block);
                llvm::IRBuilder<> builder(insert_BB);
                llvm::IntegerType *int_type = builder.getInt32Ty();
                llvm::AllocaInst* alloc_ins = builder.CreateAlloca(int_type);
                llvm::StoreInst* str_ins = builder.CreateStore(
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 666), 
                    alloc_ins
                    );
                llvm::LoadInst* load_ins = builder.CreateLoad(alloc_ins);

                // Create branch ins to successor Basic Block
                llvm::BranchInst* end_of_insert_BB = builder.CreateBr(orig_suc_block);

                llvm::Instruction* val_ins = nullptr;
                for (llvm::Instruction& I : **b) {
                    if (llvm::isa<llvm::LoadInst>(&I) || llvm::isa<llvm::BinaryOperator>(&I)) {
                        if (I.getType() == llvm::Type::getInt32Ty(context)) {
                            val_ins = &I;
                            break;
                        }
                    }
                }

                /* Try to make the unconditional branch conditional  */
                llvm::IRBuilder<> builder1(*b);
                (*b)->getTerminator()->eraseFromParent(); // Delete the original Terminator. Don't use removeFromParent
                if (val_ins) {
                    
                    // Make a similar clone BB, do NOT use llvm::CloneBasicBlock
                    /*
                    llvm::BasicBlock *clone_BB = llvm::BasicBlock::Create(context, "clone_BB", &F, insert_BB);
                    llvm::IRBuilder<> builder2(clone_BB);
                    llvm::IntegerType *int_type1 = builder2.getInt32Ty();
                    llvm::AllocaInst* alloc_ins1 = builder2.CreateAlloca(int_type);
                    llvm::StoreInst* str_ins1 = builder2.CreateStore(
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 888),  alloc_ins1);
                    llvm::LoadInst* load_ins1 = builder2.CreateLoad(alloc_ins1);
                    llvm::BranchInst* end_of_clone_BB = builder2.CreateBr(orig_suc_block);
                    */

                    llvm::ValueToValueMapTy vmap;
                    llvm::BasicBlock* clone_BB = llvm::CloneBasicBlock(insert_BB, vmap, "_gen", &F);

                    for (llvm::Instruction& i : *clone_BB) {
                        
                        // Fix oprands for cloned instructions
                        llvm::RemapInstruction(&i, vmap, llvm::RF_NoModuleLevelChanges | llvm::RF_IgnoreMissingLocals);

                        // Fix cloned phi instructions
                        if (llvm::PHINode *pn = llvm::dyn_cast<llvm::PHINode>(&i)) {
                            for (unsigned j = 0, e = pn->getNumIncomingValues(); j != e; ++j) {
                                llvm::Value *v = MapValue(pn->getIncomingBlock(j), vmap, llvm::RF_None, 0);
                                if (v != nullptr){
                                    pn->setIncomingBlock(j, llvm::cast<llvm::BasicBlock>(v));
                                }
                            }
                        }
                    }

                    // Make conditional jump
                    llvm::Value* rem_val = builder1.CreateURem(val_ins, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 2));
                    llvm::Value* cmp_val = builder1.CreateICmpEQ(rem_val, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
                    builder1.CreateCondBr(cmp_val, insert_BB, clone_BB);

                    // // Make phi ins
                    // llvm::Instruction* first_ins = orig_suc_block->getFirstNonPHI();
                    // llvm::IRBuilder<> builder2(first_ins);
                    // llvm::PHINode* phi_ins = builder2.CreatePHI(alloc_ins->getType(), 2);
                    // phi_ins->addIncoming(alloc_ins, insert_BB);
                    // phi_ins->addIncoming(alloc_ins, clone_BB);

                    // llvm::PHINode* phi_ins1 = builder2.CreatePHI(load_ins->getType(), 2);
                    // phi_ins1->addIncoming(load_ins, insert_BB);
                    // phi_ins1->addIncoming(load_ins, clone_BB);

                } else {
                    // Update the original last ins with unconditional branch ins
                    llvm::BranchInst *br_to_insert_BB = builder1.CreateBr(insert_BB);
                }
            }
            
            return true;
            
        }

    };

    char InsertBB::ID = 2;

    static llvm::RegisterPass<InsertBB> X("inserter", "A Insert Pass",
                                true,
                                false);

    static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                    [](const llvm::PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) { 
                                        PM.add(new InsertBB);
                                    });

}


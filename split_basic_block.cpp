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

            for (auto b = basic_blocks.begin(); b < basic_blocks.end(); ++b)
                split_BB_rec(*b);
            
            return true;
            
        }

        void split_BB_rec(llvm::BasicBlock* b)
        {
            for (auto I = b->begin(); I != b->end() ; ++I) {

                if (&(*I) == b->getTerminator() || llvm::isa<llvm::PHINode>(*I))
                    break;

                if (llvm::isa<llvm::CallInst>(I)) {
                    llvm::Instruction* next_i = nullptr;
                    if ((next_i = I->getNextNode()) != b->getTerminator()) {
                        llvm::BasicBlock* new_b = b->splitBasicBlock(next_i);
                        split_BB_rec(new_b);
                    }
                    
                }
            }
        }

    };

    char InsertBB::ID = 1;

    static llvm::RegisterPass<InsertBB> X("splitter", "A Split Pass",
                                true,
                                false);

    static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                    [](const llvm::PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) { 
                                        PM.add(new InsertBB);
                                    });

}
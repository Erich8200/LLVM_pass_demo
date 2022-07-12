#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <string>
#include <iostream>
#include <set>

namespace {

    struct DomAnalysis : public llvm::FunctionPass {
        static char ID;

        DomAnalysis() : FunctionPass(ID) { }

        virtual bool runOnFunction(llvm::Function& F) override 
        {
            llvm::DominatorTree* T = new llvm::DominatorTree(F);
            // T->runOnFunction(F);
            T->print(llvm::outs());//打印出支配树

            for (llvm::BasicBlock& cur_BB : F) {
                std::set<llvm::Value*> vals;
                std::set<llvm::Value*> visited;
                findLocalValsInSuccessors(&cur_BB, 
                                            llvm::Type::getInt32Ty(F.getContext()), 
                                            0, vals, T, &cur_BB, visited);
                llvm::outs() << cur_BB.getName() << " dom value ins:";
                for (auto it = vals.begin(); it != vals.end(); it++) {
                    llvm::outs() << **it << " ";
                }
                llvm::outs() << "\n";
            }

            return false;
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

    };

    char DomAnalysis::ID = 0;

    static llvm::RegisterPass<DomAnalysis> X("dom_ana", "Print a dominators for all blocks",
                                false,
                                false);

    static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                    [](const llvm::PassManagerBuilder &Builder, 
                                       llvm::legacy::PassManagerBase &PM) { 
                                        PM.add(new DomAnalysis);
                                    });

}

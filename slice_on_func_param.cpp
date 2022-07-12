#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/User.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"

#include <string>
#include <iostream>
#include <unordered_set>
#include <vector>

// using namespace llvm;

namespace {

    struct PrintSucAndNextNode : public llvm::FunctionPass {
        static char ID;

        std::vector<std::vector<llvm::Instruction*> > sliced_functions;

        PrintSucAndNextNode() : FunctionPass(ID) { }
        virtual bool runOnFunction(llvm::Function& F) override {

            for (llvm::BasicBlock& B : F)
            {

                for (llvm::Instruction& I : B)
                {
                    // if (isa<BinaryOperator>(I))
                    // {
                    //     outs() << "Found target ins" << I << "\n";
                    //     for (auto O = I.op_begin(); O != I.op_end(); O++)
                    //     {
                    //         outs() << *O << " ";
                    //         outs() << O->get()->getName() << "\n";
                    //     }
                    //     outs() << "\n";
                    // }
                    
                    // outs() << I.getNumUses() << " users for ins " << I << "\n";
                    // for (llvm::Value::user_iterator it = I.user_begin(); it != I.user_end(); ++it)
                    // {
                    //     if (!it->getName().empty())
                    //         outs() << it->getName() << "\n";
                    //     else {
                    //         std::string BBName("");
                    //         llvm::raw_string_ostream OS(BBName);
                    //         it->printAsOperand(OS, false);
                    //         std::cout << OS.str() << "\n";
                    //     }
                    //
                    // }
                    
                    // llvm::outs() << "Instruction " << I << "\n";
                    // llvm::outs() << "\"Operand\" count:" << I.getNumOperands() << "\n";

                    if (strcmp(I.getOpcodeName(), "call") == 0) {

                        // llvm::outs() << "Print operands: ";                    
                        // for (llvm::Instruction::op_iterator O = I.op_begin(); 
                        //                                     O != I.op_end(); O++) {   
                        //     llvm::Value* v = O->get();
                        //     if (!v->getName().empty()) {
                        //         llvm::Instruction* d_ins = nullptr;
                        //         if (d_ins = llvm::dyn_cast<llvm::Instruction>(v)) {
                        //             llvm::outs() << "Defined in this instruction:" << *d_ins 
                        //             << "\n";
                        //         }
                        //     }
                        // }
                        
                        /* Program slice */
                        std::vector<llvm::Instruction*> sliced_instructions;
                        slice_forward_rec(&I, sliced_instructions);
                    }


                    // outs() << "Print uses:\n";
                    // for (llvm::Value::use_iterator use = I.use_begin(); use != I.use_end(); use++)
                    // {
                    //     outs() << use->get()->getName() << " ";
                    // }

                    // outs() << "\n";

                }
            }

            /* Print slice */
            for (auto it = sliced_functions.begin(); it != sliced_functions.end(); ++it) {
                for (auto i = it->begin(); i != it->end(); ++i) {
                    llvm::outs() << **i << "\n";
                }
                llvm::outs() << "\n";
            }

            return false;
        }

        void slice_forward_rec(llvm::Instruction* ins, 
                               std::vector<llvm::Instruction*>& selected_insns)
        {
            // if (strcmp(ins->getOpcodeName(), "alloca") == 0)
            //     sliced_functions.push_back(selected_insns);

            for (llvm::Instruction::op_iterator use = ins->op_begin(); 
                                            use != ins->op_end(); ++use) {
                llvm::Value* v = use->get();
                if (llvm::isa<llvm::Instruction>(v)) {
                    llvm::Instruction* d_ins = llvm::dyn_cast<llvm::Instruction>(v);
                    // llvm::outs() << *d_ins << "\n";
                    selected_insns.push_back(d_ins);
                    slice_forward_rec(d_ins, selected_insns);
                    if (ins->getNumOperands() <= 1)
                        sliced_functions.push_back(selected_insns);
                    selected_insns.pop_back();
                } else if (llvm::isa<llvm::GlobalVariable>(v)) {
                    // llvm::outs() << *llvm::dyn_cast<llvm::GlobalVariable>(v) << "\n";
                    sliced_functions.push_back(selected_insns);

                    // for (auto it = selected_insns.begin(); it != selected_insns.end(); it++)
                    // {
                    //     llvm::outs() << **it << "\n";
                    // }
                    

                }
            }

            // llvm::outs() << *ins << "\n";
            // sliced_functions.push_back(selected_insns);            
        }

    };

    char PrintSucAndNextNode::ID = 0;

    static llvm::RegisterPass<PrintSucAndNextNode> X("slicer", "A Slice Pass",
                                false /* Only looks at CFG */,
                                false /* Analysis Pass */);

    static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                    [](const llvm::PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) { 
                                        PM.add(new PrintSucAndNextNode);
                                    });

}
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
#include <llvm/IR/Function.h>
#include <llvm/Support/SHA1.h>
#include <llvm/Support/Base64.h>

#include <string>
#include <iostream>

namespace {

    struct ObfFuncName : public llvm::ModulePass {
        static char ID;

        ObfFuncName() : ModulePass(ID) { }

        virtual bool runOnModule(llvm::Module& M) override {
            for (llvm::Function& F : M) {
                llvm::StringRef orig_func_name = F.getName();
                if (orig_func_name != "main" && !F.empty()) {

                    // Generate function name's SHA-1 base64 string
                    llvm::SHA1 sha1;
                    sha1.update(orig_func_name);
                    std::string new_func_name = 
                            llvm::encodeBase64<llvm::StringRef>(sha1.final());

                    // Perify new function name
                    for (size_t i = 0; i < new_func_name.size(); i++) {
                        if (new_func_name[i] == '/' || new_func_name[i] == '=')
                            new_func_name[i] = '_';
                        else if (new_func_name[i] == '+')
                            new_func_name[i] = '$';
                    }
                    
                    F.setName(new_func_name);

                    llvm::outs() << "Changed function name from " << orig_func_name 
                        << " to " << new_func_name << "\n";
                }
            }
            
            return true;
        }

    };

    char ObfFuncName::ID = 0;

    static llvm::RegisterPass<ObfFuncName> X("obf_func_name", "Obfuscate functions' name",
                                true,
                                false);

    static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible, 
                                    [](const llvm::PassManagerBuilder &Builder, 
                                       llvm::legacy::PassManagerBase &PM) { 
                                        PM.add(new ObfFuncName);
                                    });

}


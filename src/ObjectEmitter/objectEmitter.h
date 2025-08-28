#pragma once

#include "../CompilerPass.h"
#include "../AST/astVisitor.h"

#include <llvm/Target/TargetMachine.h>

struct ExecResult{
    std::string output;
    int  exitCode;
};

class ObjectEmitter : private CompilerPass{
public:
    ObjectEmitter(SourceFile *sourceFile,llvm::TargetMachine *targetMachine) : CompilerPass(sourceFile) , llvmModule(*sourceFile->llvmModule) {};
    void emit() const;
    void link() const;
    
private:
    llvm::Module &llvmModule;
    llvm::TargetMachine *targetMachine;

    ExecResult exec(const std::string &cmd) const;


};
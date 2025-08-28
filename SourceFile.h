#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include "symbolTable/SymbolTable.h"

using namespace std;
class SourceFile {
public:
    explicit SourceFile(const std::filesystem::path &filePath);
    void parse();
    void createSymbolTable();
    void typeCheck();
    void generateIR();
    void optimizeIR();
    void emitObject();
    std::filesystem::path filePath;
    SymbolTable symbolTable;
    vector<unique_ptr<ASTNode>> astNodes;
    llvm::LLVMContext context;
    unique_ptr<llvm::Module> llvmModule;
    unique_ptr<llvm::TargetMachine> targetMachine;

private:
    ASTEntryNode *ast = nullptr;
};
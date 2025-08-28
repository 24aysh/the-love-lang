#pragma once

#include <string>

#include <llvm/IR/Value.h>
#include <SymbolType.h>

using namespace std;

class ASTNode;

class SymbolTableEntry{
public:
    string name;
    ASTNode *declNode;
    SymbolType type = SymbolType(TY_INVALID);
    llvm::Value *address;
    SymbolTableEntry(string name,SymbolType type,ASTNode *declNode){
        this->name = std::move(name);
        this->type = std::move(type);
        this->declNode = declNode; 
    }
};
#pragma once

#include "../SourceFile.h"
#include "../AST/astNode.h"
#include "SymbolTableEntry.h"
#include <unordered_map>
#include <string>

using namespace std;

class SourceFile;

class SymbolTable{
public:
    explicit SymbolTable(SourceFile *sourceFile){
        this->sourceFile = sourceFile;
    }
    SymbolTableEntry* insert(const string &,ASTNode* declNode);
    SymbolTableEntry* lookup(const string &name);
private:
    SourceFile* sourceFile;
    unordered_map<string,SymbolTableEntry> symbols;
};
#include "SymbolTableBuilder.h"

#include <stdexcept>

std::any SymbolTableBuilder::visitDeclStmt(ASTDeclStmtNode *node){
    const std::string &name = node->varName;
    SymbolTable &symbolTable = sourceFile->symbolTable;
    
    if(symbolTable.lookup(name)){
        throw std::runtime_error("Re-declaration of variable '" + name + "'");
    }
    node->varEntry = symbolTable.insert(name,node);
    return nullptr;

}
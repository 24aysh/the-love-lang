#include "typeChecker.h"

#include <cassert>
#include <stdexcept>
#include <any>

any TypeChecker::visitDeclStmt(ASTDeclStmtNode *node){
    auto dataType = std::any_cast<SymbolType>(visit(node->dataType()));
    auto initValue = std::any_cast<ExprResult>(visit(node->additiveExpr()));
    if(initValue.type != dataType){
        std::runtime_error(node->codeLoc.print() + ": Cannot assign an " + initValue.type.getName() + " value to " + dataType.getName() + " variable");
    }
    SymbolTableEntry *varEntry = sourceFile->symbolTable.lookup(node->varName);
    assert(varEntry != nullptr);
    varEntry->type = dataType;

    node->setEvaluatedSymbolType(dataType);
    return ExprResult{dataType, varEntry};


};
std::any TypeChecker::visitAdditiveExpr(ASTAdditiveExprNode *node) {
    if (node->children.size() == 1){
        return visit(node->operands().front());
    }
    auto lhs = std::any_cast<ExprResult>(visit(node->operands().at(0)));
    auto rhs = std::any_cast<ExprResult>(visit(node->operands().at(1)));
    if (lhs.type != rhs.type){
        throw std::runtime_error(node->codeLoc.print() + ": Cannot apply + operator on " + lhs.type.getName() + " and " + rhs.type.getName());
    }
    node->setEvaluatedSymbolType(lhs.type);
    return ExprResult{lhs.type};
  }

std::any TypeChecker::visitMultiplicativeExpr(ASTMultiplicativeExprNode *node) {
    if (node->children.size() == 1){
        return visit(node->operands().front());
    }
      
    auto lhs = std::any_cast<ExprResult>(visit(node->operands().at(0)));
    auto rhs = std::any_cast<ExprResult>(visit(node->operands().at(1)));
    if (lhs.type != rhs.type){
        throw std::runtime_error(node->codeLoc.print() + ": Cannot apply * operator on " + lhs.type.getName() + " and " + rhs.type.getName());
    }
      
    node->setEvaluatedSymbolType(lhs.type);
    return ExprResult{lhs.type};
}

std::any TypeChecker::visitAtomicExpr(ASTAtomicExprNode *node) {
    if (node->constant()){
        return visit(node->constant());
    }
        

    if (node->additiveExpr()){
        return visit(node->additiveExpr());
    }
        
    const std::string &name = node->referencedVariableName;
    assert(!name.empty());
    node->referencedEntry = sourceFile->symbolTable.lookup(name);
    if (!node->referencedEntry){
        throw std::runtime_error(node->codeLoc.print() + ": Referenced undefined variable '" + name + "'");
    }
        
    const SymbolType &varType = node->referencedEntry->type;
    assert(!varType.is(TY_INVALID));

    node->setEvaluatedSymbolType(varType);
    return ExprResult{varType, node->referencedEntry};
}

std::any TypeChecker::visitConstant(ASTConstantNode *node) {
    SymbolType symbolType(TY_INVALID);
    if (node->type == ASTConstantNode::TYPE_INT)
        symbolType = SymbolType(TY_INT);
    else if (node->type == ASTConstantNode::TYPE_DOUBLE)
        symbolType = SymbolType(TY_DOUBLE);
    else
        assert(false && "TypeChecker - Constant fall-through");

    node->setEvaluatedSymbolType(symbolType);
    return ExprResult{symbolType};
}

std::any TypeChecker::visitDataType(ASTDataTypeNode *node) {
    SymbolType symbolType(TY_INVALID);
    if (node->type == ASTDataTypeNode::TYPE_INT)
        symbolType = SymbolType(TY_INT);
    else if (node->type == ASTDataTypeNode::TYPE_DOUBLE)
        symbolType = SymbolType(TY_DOUBLE);
    else
        assert(false && "TypeChecker - DataType fall-through");

    node->setEvaluatedSymbolType(symbolType);
    return symbolType;
}
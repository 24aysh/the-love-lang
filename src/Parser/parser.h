#pragma once

#include <stack>
#include <memory>

#include "../CompilerPass.h"
#include "../AST/astNode.h"
#include "../Lexer/Lexer.h"

class Parser : public CompilerPass{
public:

    Parser(SourceFile *sourceFile, Lexer &lexer) : CompilerPass(sourceFile), lexer(lexer) {}
    ASTEntryNode *parse();
    ASTStmtNode *parseStmt();
    ASTDeclStmtNode *parseDeclStmt();
    ASTAdditiveExprNode *parseAdditiveExpr();
    ASTMultiplicativeExprNode *parseMultiplicativeExpr();
    ASTAtomicExprNode *parseAtomicExpr();
    ASTConstantNode *parseConstant();
    ASTPrintCallNode *parsePrintCall();
    ASTDataTypeNode *parseDataType();

private:
    Lexer &lexer;
    stack<ASTNode *> parentStack;
    
    template <typename T> T *createNode();
    template <typename T> T *concludeNode(T *node);
};

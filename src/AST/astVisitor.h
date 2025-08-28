#pragma once

#include <any>

using namespace std;

class ASTNode;
class ASTEntryNode;
class ASTStmtNode;
class ASTDeclStmtNode;
class ASTAdditiveExprNode;
class ASTMultiplicativeExprNode;
class ASTAtomicExprNode;
class ASTConstantNode;
class ASTPrintCallNode;
class ASTDataTypeNode;

class ASTVisitor {
public:
  
  any visit(ASTNode *node);
  any visitChildren(ASTNode *node);

  virtual any visitEntry(ASTEntryNode *node);
  virtual any visitStmt(ASTStmtNode *node);
  virtual any visitDeclStmt(ASTDeclStmtNode *node);
  virtual any visitAdditiveExpr(ASTAdditiveExprNode *node);
  virtual any visitMultiplicativeExpr(ASTMultiplicativeExprNode *node);
  virtual any visitAtomicExpr(ASTAtomicExprNode *node);
  virtual any visitConstant(ASTConstantNode *node);
  virtual any visitPrintCall(ASTPrintCallNode *node);
  virtual any visitDataType(ASTDataTypeNode *node);
};
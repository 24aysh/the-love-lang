#pragma once

#include <any>
#include <string>
#include <vector>

#include "../AST/astVisitor.h"
#include "../Reader/Reader.h"
#include "../symbolTable/SymbolTableEntry.h"

class ASTNode {
public:
 
  ASTNode(ASTNode *parent, CodeLoc codeLoc) : parent(parent), codeLoc(std::move(codeLoc)) {}
  ASTNode(const ASTNode &) = delete;

 
  virtual std::any accept(ASTVisitor *visitor) = 0;

  
  template <typename T> void addChild(T *node) {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from ASTNode");
    children.push_back(node);
  }

  template <typename T> [[nodiscard]] T *getChild(size_t i = 0) const {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from ASTNode");
    size_t j = 0;
    for (ASTNode *child : children) {
      if (auto *typedChild = dynamic_cast<T *>(child)) [[unlikely]] {
        if (j++ == i)
          return typedChild;
      }
    }
    return nullptr;
  }

  template <typename T> [[nodiscard]] std::vector<T *> getChildren() const {
    static_assert(std::is_base_of_v<ASTNode, T>, "T must be derived from ASTNode");
    std::vector<T *> nodes;
    for (ASTNode *child : children) {
      if (auto *typedChild = dynamic_cast<T *>(child)) [[unlikely]] {
        nodes.push_back(typedChild);
      }
    }
    return nodes;
  }

  void setEvaluatedSymbolType(const SymbolType &symbolType) { this->symbolType = symbolType; }

  [[nodiscard]] const SymbolType &getEvaluatedSymbolType() const { // NOLINT(misc-no-recursion)
    if (!symbolType.is(TY_INVALID))
      return symbolType;
    if (children.size() != 1)
      throw std::runtime_error("Cannot deduce evaluated symbol type");
    return children.front()->getEvaluatedSymbolType();
  }

  
  ASTNode *parent;
  std::vector<ASTNode *> children;
  const CodeLoc codeLoc;
  SymbolType symbolType = SymbolType(TY_INVALID);
};

class ASTEntryNode : public ASTNode {
public:
  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitEntry(this); }

  [[nodiscard]] std::vector<ASTStmtNode *> stmts() const { return getChildren<ASTStmtNode>(); }
};

class ASTStmtNode : public ASTNode {
public:
  using ASTNode::ASTNode;
  std::any accept(ASTVisitor *visitor) override { return visitor->visitStmt(this); }
};

class ASTDeclStmtNode : public ASTNode {
public:
  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitDeclStmt(this); }

  [[nodiscard]] ASTDataTypeNode *dataType() const { return getChild<ASTDataTypeNode>(); }
  [[nodiscard]] ASTAdditiveExprNode *additiveExpr() const { return getChild<ASTAdditiveExprNode>(); }

  std::string varName;
  SymbolTableEntry *varEntry = nullptr;
};

class ASTAdditiveExprNode : public ASTNode {
public:
  enum AdditiveOp : int8_t {
    OP_NONE,
    OP_PLUS,
    OP_MINUS,
  };

  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitAdditiveExpr(this); }

  [[nodiscard]] std::vector<ASTMultiplicativeExprNode *> operands() const { return getChildren<ASTMultiplicativeExprNode>(); }

  AdditiveOp op = OP_NONE;
};

class ASTMultiplicativeExprNode : public ASTNode {
public:
  enum MultiplicativeOp : int8_t {
    OP_NONE,
    OP_MUL,
    OP_DIV,
  };

  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitMultiplicativeExpr(this); }

  [[nodiscard]] std::vector<ASTAtomicExprNode *> operands() const { return getChildren<ASTAtomicExprNode>(); }

  MultiplicativeOp op = OP_NONE;
};

class ASTAtomicExprNode : public ASTNode {
public:
  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitAtomicExpr(this); }

  [[nodiscard]] ASTConstantNode *constant() const { return getChild<ASTConstantNode>(); }
  [[nodiscard]] ASTAdditiveExprNode *additiveExpr() const { return getChild<ASTAdditiveExprNode>(); }

  std::string referencedVariableName;
  SymbolTableEntry *referencedEntry;
};

class ASTConstantNode : public ASTNode {
public:
  enum Type {
    TYPE_NONE,
    TYPE_INT,
    TYPE_DOUBLE,
  };

  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitConstant(this); }

  Type type = TYPE_NONE;
  union {
    int32_t intValue;
    double doubleValue;
  } compileTimeVaue;
};

class ASTPrintCallNode : public ASTNode {
public:
  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitPrintCall(this); }

  [[nodiscard]] ASTAdditiveExprNode *arg() const { return getChild<ASTAdditiveExprNode>(); }
};

class ASTDataTypeNode : public ASTNode {
public:
  enum Type {
    TYPE_NONE,
    TYPE_INT,
    TYPE_DOUBLE,
  };

  using ASTNode::ASTNode;

  std::any accept(ASTVisitor *visitor) override { return visitor->visitDataType(this); }

  Type type = TYPE_NONE;
};
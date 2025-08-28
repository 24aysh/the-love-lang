#include "parser.h"

#include <assert.h>

ASTEntryNode *Parser::parse(){
    ASTEntryNode *entryNode = createNode<ASTEntryNode>();
    while (lexer.getToken().type != TOK_EOF)
    {
        parseStmt();
    }
    return concludeNode(entryNode);
}

ASTStmtNode *Parser::parseStmt(){
    ASTStmtNode *stmtnode = createNode<ASTStmtNode>();
    TokenType curTok = lexer.getToken().type;
    if(curTok == TOK_DOUBLE_LIT || curTok == TOK_INT_LIT){
        parseDeclStmt();
    }
    else{
        parsePrintCall();
    }
    lexer.expect(TOK_SEMICOLON);

    return concludeNode(stmtnode);
}

ASTDeclStmtNode *Parser::parseDeclStmt(){
    ASTDeclStmtNode *declStmtNode = createNode<ASTDeclStmtNode>();

    parseDataType();
    const Token &token = lexer.getToken();
    declStmtNode->varName = token.text;

    lexer.expect(TOK_IDENTIFIER);
    lexer.expect(TOK_ASSIGN);

    parseAdditiveExpr();

    return concludeNode(declStmtNode);

}

ASTAtomicExprNode *Parser::parseAtomicExpr(){
    ASTAtomicExprNode *node = createNode<ASTAtomicExprNode>();

    const TokenType currentToken = lexer.getToken().type;

    if(currentToken == TOK_DOUBLE_LIT || currentToken == TOK_INT_LIT){
        parseConstant();
    }
    else if( currentToken == TOK_IDENTIFIER){
        node->referencedVariableName = lexer.getToken().text;
        lexer.expect(TOK_IDENTIFIER);
    }
    else if(currentToken == TOK_LPAREN){
        lexer.expect(TOK_LPAREN);
        parseAdditiveExpr();
        lexer.expect(TOK_RPAREN);
    }

    return concludeNode(node);
}

ASTAdditiveExprNode *Parser::parseAdditiveExpr(){
    ASTAdditiveExprNode *node = createNode<ASTAdditiveExprNode>();

    parseMultiplicativeExpr();

    const Token &token = lexer.getToken();

    if(token.type == TOK_MINUS || token.type == TOK_PLUS){
        if(token.type == TOK_MINUS){
            node->op = ASTAdditiveExprNode::OP_MINUS;
        }
        else if(token.type == TOK_PLUS){
            node->op = ASTAdditiveExprNode::OP_PLUS;
        }
        lexer.expectOneOf({TOK_PLUS,TOK_MINUS});
        
        parseMultiplicativeExpr();
    }
    return concludeNode(node);

}

ASTMultiplicativeExprNode *Parser::parseMultiplicativeExpr(){
    ASTMultiplicativeExprNode *node = createNode<ASTMultiplicativeExprNode>();

    parseAtomicExpr();

    const Token &currentToken = lexer.getToken();
    if(currentToken.type == TOK_MUL || currentToken.type == TOK_DIV){
        if(currentToken.type == TOK_MUL){
            node->op = ASTMultiplicativeExprNode::OP_MUL;
        }
        else if(currentToken.type == TOK_DIV){
            node->op = ASTMultiplicativeExprNode::OP_DIV;
        }
        lexer.expectOneOf({TOK_MUL,TOK_DIV});
    }
    return concludeNode(node);
    
}

ASTConstantNode *Parser::parseConstant(){
    ASTConstantNode *constantNode = createNode<ASTConstantNode>();

    const Token &currentToken = lexer.getToken();
    if(currentToken.type == TOK_INT_LIT){
        constantNode->type = ASTConstantNode::TYPE_INT;
        constantNode->compileTimeVaue.intValue = std::stoi(currentToken.text);
    }
    else if(currentToken.type == TOK_DOUBLE_LIT){
        constantNode->type = ASTConstantNode::TYPE_DOUBLE;
        constantNode->compileTimeVaue.doubleValue = std::stod(currentToken.text);
    }
    lexer.expectOneOf({TOK_DOUBLE_LIT,TOK_INT_LIT});

    return concludeNode(constantNode);

}

ASTPrintCallNode *Parser::parsePrintCall(){
    ASTPrintCallNode *printCallNode = createNode<ASTPrintCallNode>();

    const Token &token = lexer.getToken();
    lexer.expect(TOK_PRINT);
    lexer.expect(TOK_LPAREN);
    parseAdditiveExpr();
    lexer.expect(TOK_RPAREN);
    
    return concludeNode(printCallNode);
}

ASTDataTypeNode *Parser::parseDataType(){
    ASTDataTypeNode *node = createNode<ASTDataTypeNode>();

    const Token &token = lexer.getToken();
    if(token.type == TOK_INT_LIT){
        node->type = ASTDataTypeNode::TYPE_INT;
    }
    else if(token.type == TOK_DOUBLE_LIT){
        node->type = ASTDataTypeNode::TYPE_DOUBLE;
    }
    lexer.expectOneOf({TOK_DOUBLE_LIT,TOK_INT_LIT});

    return concludeNode(node);
}

template <typename T> T *Parser::createNode() {
  ASTNode *parent = nullptr;
  if constexpr (!std::is_same_v<T, ASTEntryNode>)
    parent = parentStack.top();

  sourceFile->astNodes.push_back(std::make_unique<T>(parent, lexer.getCodeLoc()));
  T *node = static_cast<T *>(sourceFile->astNodes.back().get());

  if constexpr (!std::is_same_v<T, ASTEntryNode>)
    parent->addChild(node);

  parentStack.push(node);

  return node;
}

template <typename T> T *Parser::concludeNode(T *node) {
  assert(parentStack.top() == node);
  parentStack.pop();

  return node;
}
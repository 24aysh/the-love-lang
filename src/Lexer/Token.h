#pragma once

#include <cstdint>
#include <string>

#include "../Reader/Reader.h"

using namespace std;

enum TokenType {
  TOK_INVALID,
  TOK_INT_LIT,
  TOK_DOUBLE_LIT,
  TOK_IDENTIFIER,
  TOK_TYPE_INT,
  TOK_TYPE_DOUBLE,
  TOK_PRINT,
  TOK_ASSIGN,
  TOK_PLUS,
  TOK_MINUS,
  TOK_MUL,
  TOK_DIV,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_SEMICOLON,
  TOK_EOF
};
struct Token
{
  TokenType type;
  string text;
  CodeLoc codeLoc;
  explicit Token(TokenType type);
  Token(TokenType type,const string &text,const CodeLoc &codeloc){
    this->text=text;
    this->type=type;
    this->codeLoc=codeLoc;
  }
};

#include "Lexer.h"
#include <algorithm>
#include <cassert>
#include <sstream>

Lexer::Lexer(SourceFile *sourceFile) : CompilerPass(sourceFile){
    advance();
};

const Token &Lexer::getToken() const {
    return curTok;
}

void Lexer::advance(){
    while(isspace(reader.getChar()) && !reader.isEOF()){
        reader.advance();
    }
    curTok = consumeToken();
}

void  Lexer::expect(TokenType expectedType){
    assert(curTok.type == expectedType);
    advance();
}


void Lexer::expectOneOf(const std::initializer_list<TokenType> &expectedTypes) {
    const auto matcher = [&](const TokenType &expectedType) { return curTok.type == expectedType; };
    assert(std::any_of(expectedTypes.begin(), expectedTypes.end(), matcher));
    advance();
}

bool Lexer::isEOF() const {
    return reader.isEOF();
}

CodeLoc Lexer::getCodeLoc() const {
    return reader.getCodeLoc();
}

Token Lexer::consumeToken(){
    const char currentChar = reader.getChar();
    
    if(reader.isEOF()){
        return Token(TOK_EOF,"EOF",reader.getCodeLoc());
    }
    if (isalpha(currentChar) || currentChar == '_'){
        return consumeKeywordOrIdentifier();
    }
        
    if (isdigit(currentChar)){
        return consumeNumber();
    }
    reader.advance();
    switch (currentChar)
    {
    case '+':   
        return Token(TOK_PLUS,"+",reader.getCodeLoc());
    case '-':
        return Token(TOK_MINUS,"-",reader.getCodeLoc());
    case '*':
        return Token(TOK_MUL,"*",reader.getCodeLoc());
    case '/':
        return Token(TOK_DIV,"/",reader.getCodeLoc());
    case ';':
        return Token(TOK_SEMICOLON,";",reader.getCodeLoc());
    case '=':
        return Token(TOK_ASSIGN,"=",reader.getCodeLoc());
    case '(':
        return Token(TOK_RPAREN,"(",reader.getCodeLoc());
    case ')':
        return Token(TOK_LPAREN,")",reader.getCodeLoc()); 
    }

    assert(false && "UnexpectedChar");
}

Token Lexer::consumeNumber(){
    std::stringstream number;
    const CodeLoc codeLoc = reader.getCodeLoc();

    bool seenDot = false;

    do {
        char c = reader.getChar();
        if(c=='.'){
            seenDot = 1;
        }
        number << c;
        reader.advance();

    } while (isdigit(reader.getChar()) || (reader.getChar() == '.' && !seenDot));

    return Token(seenDot ? TOK_DOUBLE_LIT : TOK_INT_LIT,number.str(),codeLoc);
}

Token Lexer::consumeSemiColon(){
    reader.expect(TOK_SEMICOLON);
    return Token(TOK_SEMICOLON,";",reader.getCodeLoc());
}

Token Lexer::consumeKeywordOrIdentifier(){
    std::stringstream stream;
    CodeLoc codeloc = reader.getCodeLoc();
    do{
        stream << reader.getChar();
        reader.advance();
    } while(isalnum(reader.getChar()) || reader.getChar() == '_');

    const std::string &text = stream.str();
    if(text == "int"){
        return Token(TOK_INT_LIT,"int",codeloc);
    }
    if(text == "printf"){
        return Token(TOK_PRINT,"printf",codeloc);
    }
    if(text == "double"){
        return Token(TOK_DOUBLE_LIT,"double",codeloc);
    }
    return Token(TOK_IDENTIFIER,"identifier",codeloc);
}
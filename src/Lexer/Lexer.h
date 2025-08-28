#pragma once

#include "../Reader/Reader.h"
#include "../CompilerPass.h"
#include "Token.h"

class Lexer : public CompilerPass{
public:
    explicit Lexer(SourceFile *sourceFile);
    
    const Token &getToken() const;
    void advance();
    void expect(TokenType expectedType);
    void expectOneOf(const std::initializer_list<TokenType> &expectedTypes);
    bool isEOF() const;
    CodeLoc getCodeLoc() const;

private:
    Reader reader = Reader(sourceFile->filePath);
    Token curTok = Token(TOK_INVALID);

    Token consumeToken();
    Token consumeNumber();
    Token consumeKeywordOrIdentifier();
    Token consumeSemiColon();
    
};
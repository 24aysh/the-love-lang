#pragma once

#include "../AST/astVisitor.h"
#include "../CompilerPass.h"
#include <any>

class SymbolTableBuilder : public ASTVisitor, private CompilerPass{
public:
    explicit SymbolTableBuilder(SourceFile *sourceFile) : CompilerPass(sourceFile) {};
    any visitDeclStmt(ASTDeclStmtNode *node) override;

};
#pragma once

#include <initializer_list>
#include <string>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

using namespace std;
enum SymbolSuperType{
    TY_INVALID,
    TY_INT,
    TY_DOUBLE
};

class SymbolType{
public:
    explicit SymbolType(SymbolSuperType superType){
        superType = superType;
    }

    bool is(SymbolSuperType) const;
    bool isOneOf(const initializer_list<SymbolSuperType>&) const ;
    string getName() const;
    llvm::Type *toLLVMType(llvm::LLVMContext &context) const;
    friend bool operator==(const SymbolType&, const SymbolType&);
    friend bool operator!=(const SymbolType&, const SymbolType&);
private:
    SymbolSuperType superType;
};
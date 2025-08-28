#include "irGenerator.h"

#include <cassert>

#include <llvm/IR/Verifier.h>
#include <llvm/TargetParser/Host.h>

std::any IRGenerator::visitEntry(ASTEntryNode *node){
    llvmModule->setTargetTriple(llvm::sys::getDefaultTargetTriple());
    llvmModule->setDataLayout(sourceFile->targetMachine->createDataLayout());
    
    //create main function
    llvm::Type *returnType = builder.getInt32Ty();
    llvm::FunctionType *fcType = llvm::FunctionType::get(returnType,{},false);
    llvm::Function *fc = llvm::Function::Create(fcType,llvm::Function::ExternalLinkage,"main",llvmModule);
    fc->setDSOLocal(true);

    fc->addFnAttr(llvm::Attribute::NoInline);
    fc->addFnAttr(llvm::Attribute::NoUnwind);
    fc->addFnAttr(llvm::Attribute::getWithUWTableKind(context, llvm::UWTableKind::Default));

    //create entry block

    entryBlock = llvm::BasicBlock::Create(context);
    fc->insert(fc->end(),entryBlock);
    builder.SetInsertPoint(entryBlock);

    //visit program
    visitChildren(node);
    
    // Always return with exit code 0
    builder.CreateRet(builder.getInt32(0));


    //verify main function
    std::string output;
    llvm::raw_string_ostream oss(output);

    if(llvm::verifyFunction(*fc,&oss)){
        throw std::runtime_error("Invalid main function");
    }
    return nullptr;
}

std::any IRGenerator::visitDeclStmt(ASTDeclStmtNode *node){
    //get variable entry

    SymbolTableEntry *varEntry = node->varEntry;
    assert(varEntry !=nullptr);
    const SymbolType &varSymbolType = varEntry->type;

    llvm::Type *llvmType = varSymbolType.toLLVMType(context);
    llvm::Value *varAddress = insertAlloca(llvmType,varEntry->name + ".addr");

    varEntry->address = varAddress;

    //visit rhs
    auto initValue = std::any_cast<LLVMExprResult>(visit(node->additiveExpr()));
    assert(initValue.value !=nullptr);

    //store the rhs value to the allocated address

    builder.CreateStore(initValue.value,varAddress);

    return LLVMExprResult{.value = initValue.value,.entry = varEntry};
}

std::any IRGenerator::visitAdditiveExpr(ASTAdditiveExprNode *node){
     // Check if only one operand is present -> loop through
    if(node->operands().size()==1){
        return visit(node->operands().front());
    }

    //it is an additive expression
    //evaluate first operand

    ASTMultiplicativeExprNode *lhsNode = node->operands().at(0);
    SymbolType lhsSTy = lhsNode->getEvaluatedSymbolType();
    auto lhs = std::any_cast<LLVMExprResult>(visit(lhsNode));
    
    //evaluate second operand
    ASTMultiplicativeExprNode *rhsNode = node->operands().at(1);
    SymbolType rhsSTy = rhsNode->getEvaluatedSymbolType();
    auto rhs = std::any_cast<LLVMExprResult>(visit(rhsNode));

    assert(lhsSTy==rhsSTy);
    assert(node->op != ASTAdditiveExprNode::OP_NONE);

    llvm::Value *resultValue = nullptr;
    if(node->op == ASTAdditiveExprNode::OP_PLUS){
        if(lhsSTy.is(TY_INT)){
            resultValue = builder.CreateAdd(lhs.value,rhs.value);
        }
        else if(lhsSTy.is(TY_DOUBLE)){
            resultValue = builder.CreateFAdd(lhs.value,rhs.value);
        }
    }
    else if(node->op == ASTAdditiveExprNode::OP_MINUS){
        if(lhsSTy.is(TY_INT)){
            resultValue = builder.CreateSub(lhs.value,rhs.value);
        }
        else if(lhsSTy.is(TY_DOUBLE)){
            resultValue = builder.CreateFSub(lhs.value,rhs.value);
        }
    }
    assert(resultValue != nullptr);
    return LLVMExprResult{.value = resultValue};
}

std::any IRGenerator::visitMultiplicativeExpr(ASTMultiplicativeExprNode *node){
    //check if only one operand is present -> loop through
    if(node->operands().size()==1){
        return visit(node->operands().front());
    }

    ASTAtomicExprNode *lhsNode = node->operands().at(0); 
    SymbolType lhsSTy = lhsNode->getEvaluatedSymbolType();
    auto lhs = std::any_cast<LLVMExprResult>(visit(lhsNode));

    ASTAtomicExprNode *rhsNode = node->operands().at(1);
    SymbolType rhsSTy = rhsNode->getEvaluatedSymbolType();
    auto rhs = std::any_cast<LLVMExprResult>(visit(rhsNode));

    assert(lhsSTy == rhsSTy);
    assert(node->op != ASTMultiplicativeExprNode::OP_NONE);

    llvm::Value *result = nullptr;
    if(node->op == ASTMultiplicativeExprNode::OP_MUL){
        if(lhsSTy.is(TY_INT)){
            result = builder.CreateMul(lhs.value, rhs.value);
        }
        else if(rhsSTy.is(TY_DOUBLE)){
            result = builder.CreateFMul(lhs.value,rhs.value);
        }
    }
    else if(node->op == ASTMultiplicativeExprNode::OP_DIV){
        if(lhsSTy.is(TY_INT)){
            result = builder.CreateSDiv(lhs.value,rhs.value);
        }
        else if(rhsSTy.is(TY_DOUBLE)){
            result = builder.CreateFDiv(lhs.value,rhs.value);
        }
    }

    assert(result != nullptr);
    return LLVMExprResult{.value = result};
}

std::any IRGenerator::visitAtomicExpr(ASTAtomicExprNode *node){
    if(node->constant()){
        return visit(node->constant());
    }
    if(node->additiveExpr()){
        return visit(node->additiveExpr());
    }

    SymbolTableEntry *varEntry = node->referencedEntry;
    assert(varEntry != nullptr);
    assert(varEntry->address != nullptr);

    //load the value
    llvm::Type *llvmType = varEntry->type.toLLVMType(context);
    llvm::Value *loadValue = builder.CreateLoad(llvmType,varEntry->address,varEntry->name);

    return LLVMExprResult{.value = loadValue,.entry = varEntry};
}

std::any IRGenerator::visitConstant(ASTConstantNode *node){

    llvm::Constant *value = nullptr;
    if(node->type ==  ASTConstantNode::TYPE_INT){
        value = builder.getInt32(node->compileTimeVaue.intValue);
    }
    else if(node->type == ASTConstantNode::TYPE_DOUBLE){
        value = llvm::ConstantFP::get(context, llvm::APFloat(node->compileTimeVaue.doubleValue));
    }
    else{
        assert(false && "IRGenerator - Constant fall through"); 
    }

    return LLVMExprResult{.value = value};
    
}

std::any IRGenerator::visitPrintCall(ASTPrintCallNode *node){
    auto arg = std::any_cast<LLVMExprResult>(visit(node->arg()));
    SymbolType &argType = arg.entry->type;

    //create template string

    const std::string templateString = argType.is(TY_INT) ? "%d\n" : "%f\n";
    llvm::Constant *templateStringConstant = builder.CreateGlobalStringPtr(templateString,"print.str",0,llvmModule);

    //call printf function

    llvm::Function *printFunction = getPrintfFct();
    llvm::CallInst *returnValue = builder.CreateCall(printFunction,{templateStringConstant,arg.value});

    return LLVMExprResult{.value = returnValue};
}

std::any IRGenerator::visitDataType(ASTDataTypeNode *node){
    SymbolType symbolType = node->getEvaluatedSymbolType();
    assert(!symbolType.is(TY_INVALID));
    return symbolType.toLLVMType(context);
}

std::string IRGenerator::getIRString() const {
    std::string output;
    llvm::raw_string_ostream oss((output));
    llvmModule->print(oss,nullptr);
    return oss.str();
}

llvm::Value *IRGenerator::insertAlloca(llvm::Type *llvmType,const std::string &varName){
    //save insert point
    llvm::BasicBlock *currentBlock = builder.GetInsertBlock();
    builder.SetInsertPoint(entryBlock);

    llvm::AllocaInst *allocaInst = builder.CreateAlloca(llvmType,nullptr,varName);

    //restore old insert point
    return allocaInst;
}

llvm::Function *IRGenerator::getPrintfFct(){
    llvm::Function *printfFct = getFunction("printf",builder.getInt32Ty(),builder.getPtrTy(),true);
    printfFct->addFnAttr(llvm::Attribute::NoFree);
    printfFct->addFnAttr(llvm::Attribute::NoUnwind);
    printfFct->addParamAttr(0, llvm::Attribute::NoCapture);
    printfFct->addParamAttr(0, llvm::Attribute::NoUndef);
    printfFct->addParamAttr(0, llvm::Attribute::ReadOnly);
    printfFct->addRetAttr(llvm::Attribute::NoUndef);
    return printfFct;
}

llvm::Function *IRGenerator::getFunction(const char *funcName,llvm::Type *returnType,llvm::ArrayRef<llvm::Type *> args,bool varArg) const {
    //check if function already exists in the same module
    llvm::Function *fct = llvmModule->getFunction(funcName);
    if(fct != nullptr){
        return fct;
    }
    llvm::FunctionType *opFctTy = llvm::FunctionType::get(returnType,args,varArg);
    llvmModule->getOrInsertFunction(funcName,opFctTy);
    return llvmModule->getFunction(funcName);
}


#include "irOptimizer.h"

#include <llvm/Transforms/IPO/AlwaysInliner.h>

void IROptimizer::prepare(){
    passBuilder = std::make_unique<llvm::PassBuilder>(targetMachine);
    functionAnalysisMgr.registerPass([&] {return passBuilder->buildDefaultAAPipeline();});

    passBuilder->registerModuleAnalyses(moduleAnalysisMgr);
    passBuilder->registerCGSCCAnalyses(cgsccAnalysisMgr);
    passBuilder->registerFunctionAnalyses(functionAnalysisMgr);
    passBuilder->registerLoopAnalyses(loopAnalysisMgr);
    passBuilder->crossRegisterProxies(loopAnalysisMgr, functionAnalysisMgr, cgsccAnalysisMgr, moduleAnalysisMgr);


}
void IROptimizer::optimize(){
    assert(passBuilder!=nullptr);
    llvm::OptimizationLevel llvmOptLevel = llvm::OptimizationLevel::O2;
    llvm::ModulePassManager modulePassMgr = passBuilder->buildPerModuleDefaultPipeline(llvmOptLevel);
    
    modulePassMgr.addPass(llvm::AlwaysInlinerPass());
    modulePassMgr.run(*llvmModule, moduleAnalysisMgr);

}
std::string IROptimizer::getIRString() const {
    std::string irString;
    llvm::raw_string_ostream oss(irString);
    llvmModule->print(oss, nullptr);
    return oss.str();
}

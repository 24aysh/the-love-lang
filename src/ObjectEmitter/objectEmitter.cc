#include "objectEmitter.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>

void ObjectEmitter::emit() const {
    const std::filesystem::path objectFile = sourceFile->filePath.replace_extension("o");

    std::error_code errCode;
    llvm::raw_fd_ostream stream(objectFile.string(),errCode,llvm::sys::fs::OF_None);
    if(errCode){
        throw std::runtime_error("File '" + objectFile.string() + "' could not be opened");
    }

    llvm::legacy::PassManager passManager;
    if(targetMachine->addPassesToEmitFile(passManager,stream,nullptr,llvm::CodeGenFileType::ObjectFile,false)){
        throw std::runtime_error("Target machine can't emit file of this type");
    }
    passManager.run(llvmModule);
    stream.flush();
};

void ObjectEmitter::link() const {
    const std::filesystem::path objectFile = sourceFile->filePath.replace_extension("o");
#if OS_WINDOWS
  const std::filesystem::path outputFile = sourceFile->filePath.replace_extension("exe");
#else
  const std::filesystem::path outputFile = sourceFile->filePath.replace_extension("love");
#endif

    const std::string linkerCmd = "clang -o" + outputFile.string() + " -static " + objectFile.string();
    const ExecResult result = exec(linkerCmd);
    if (!result.output.empty()){
        std::runtime_error("Linker error: " + result.output);
    }

};
ExecResult ObjectEmitter::exec(const std::string &cmd) const {
    FILE *pipe = popen(cmd.c_str(), "r");
    assert(pipe);
    char buffer[128];
    std::stringstream result;
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
        result << buffer;
    }
    int exitCode = pclose(pipe) / 256;
    return {result.str(), exitCode};
}
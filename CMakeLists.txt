cmake_minimum_required(VERSION 3.28.3)
project(love-lang)


set(CMAKE_CXX_STANDARD 20)
set(LLVM_DIR $ENV{LLVM_DIR})


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Love: Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Love: Using LLVMConfig.cmake from ${LLVM_DIR}")
message(STATUS "Love: LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(LLVM_LIBS aarch64codegen amdgpucodegen armcodegen avrcodegen bpfcodegen hexagoncodegen
        lanaicodegen loongarchcodegen mcjit mipscodegen msp430codegen nativecodegen nvptxcodegen powerpccodegen riscvcodegen
        sparccodegen systemzcodegen target vecodegen webassemblycodegen x86codegen xcorecodegen)


include_directories(src)
add_subdirectory(src)
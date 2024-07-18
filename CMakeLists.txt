cmake_minimum_required(VERSION 3.20)
project(mba-pass-LLVM)

set(LT_LLVM_INCLUDE_DIR "/usr/local" CACHE PATH "LLVM installation directory")

list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INCLUDE_DIR}/lib/cmake/llvm/")

# for LLVMConfig.cmake -> package
find_package(LLVM REQUIRED CONFIG)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

if(NOT LLVM_ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(MBAAdd SHARED MBAAdd.cpp)

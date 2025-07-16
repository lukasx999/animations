cmake_minimum_required(VERSION 3.10)

option(WEB "build for wasm" OFF)
option(DYNAMIC "build dynamic library" OFF)

if(WEB)
    set(CMAKE_CXX_COMPILER "em++")
else()
    set(CMAKE_CXX_COMPILER "clang++")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Og -ggdb -std=c++23 -pedantic")

project(anim)

list(APPEND sources sequence.cc batch.cc)

if(DYNAMIC)
    add_library(anim SHARED ${sources})
else()
    add_library(anim STATIC ${sources})
endif()

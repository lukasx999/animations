cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_COMPILER "em++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Og -ggdb -std=c++23 -pedantic")


project(anim)

list(APPEND sources sequence.cc batch.cc)

add_library(anim STATIC ${sources})
# add_library(anim_dyn SHARED ${sources})

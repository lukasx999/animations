cmake_minimum_required(VERSION 3.10)
project(anim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Og -ggdb -std=c++23 -pedantic")

list(APPEND sources sequence.cc batch.cc)

add_library(anim STATIC ${sources})
add_library(anim_dyn SHARED ${sources})

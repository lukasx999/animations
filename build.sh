#!/bin/sh
set -euxo pipefail

clang++ main.cc -o out -I./raylib-cpp/include/ -Wall -Wextra -std=c++23 -lraylib
./out

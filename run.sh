#!/bin/sh
set -euxo pipefail

clang++ main.cc -std=c++23 -lraylib -ggdb && ./a.out

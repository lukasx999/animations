#!/bin/sh
set -euxo pipefail

mkdir -p libanim/build
pushd libanim/build
cmake -GNinja .. && cmake --build .
popd

clang++ main.cc -I./libanim/ -L./libanim/build/ -lanim -std=c++23 -lraylib -ggdb -o example
./example

#!/bin/sh
set -euxo pipefail

mkdir -p libanim/build
pushd libanim/build
cmake -GNinja .. && cmake --build .
popd

clang++ main.cc -I./libanim/ -L./libanim/build/ -lanim -std=c++23 -lraylib -ggdb -o example
./example

# em++ main.cc -std=c++23 -o index.html -I./raylib/src ./raylib/src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY --shell-file ./raylib/src/minshell.html ./libanim/build/libanim.a

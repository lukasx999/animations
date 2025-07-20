#!/bin/sh
set -euo pipefail

if [[ $# != 1 ]]; then
    echo "Usage: $0 <web|native>" 1>&2
    exit 1
fi

build_type=$1
anim=../anim
cflags="-Wall -Wextra -std=c++23 -pedantic -O3"
libs="-I$anim -L$anim/build -lanim -lraylib"

build_library() {
    build_type=$1
    web=""

    if [[ $build_type == "web" ]]; then
        web=-DCMAKE_CXX_COMPILER=em++
    fi

    cmake --fresh -GNinja -B $anim/build -S $anim $web
    cmake --build $anim/build
}

build_web() {
    mkdir -p ../docs
    raylib=./raylib-5.5_webassembly/
    em++ main.cc $cflags $libs \
    -o ../docs/index.html \
    -I$raylib/include -L$raylib/lib \
    -s USE_GLFW=3 -s ASYNCIFY --shell-file ./minshell.html
}

build_native() {
    clang++ main.cc $cflags $libs -o example
    ./example
    rm ./example
}

case $build_type in
    "web")
        build_library $build_type
        build_web
        ;;
    "native")
        build_library $build_type
        build_native
        ;;
    *)
        echo "Invalid build type, try <web|native>" 1>&2
        exit 1
        ;;
esac

#!/bin/sh
set -euo pipefail

if [[ $# != 1 ]]; then
    echo "Usage: $0 <web|native>" 1>&2
    exit 1
fi

build_type=$1

function build_library {
    build_type=$1
    web=""

    if [[ $build_type == "web" ]]; then
        web=-DWEB=ON
    fi

    cmake --fresh -GNinja -B ../libanim/build -S ../libanim $web
    cmake --build ../libanim/build
}

anim=../libanim
cflags="-Wall -Wextra -std=c++23 -pedantic -O3"
libs="-I$anim -L$anim/build -lanim -lraylib"

function build_web {
    raylib=./raylib-5.5_webassembly/
    em++ main.cc $cflags $libs \
    -o ../docs/index.html \
    -I$raylib/include -L$raylib/lib \
    -s USE_GLFW=3 -s ASYNCIFY --shell-file ./minshell.html
}

function build_native {
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

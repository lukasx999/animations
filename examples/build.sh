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

function build_web {
    em++ main.cc -I../libanim/ -std=c++23 -o ../docs/index.html -I./raylib/src ./raylib/src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY --shell-file ./raylib/src/minshell.html ../libanim/build/libanim.a
}

function build_native {
    clang++ main.cc -I../libanim/ -L../libanim/build/ -lanim -std=c++23 -lraylib -ggdb -o example
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

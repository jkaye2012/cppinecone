#!/bin/bash

if [ "$1" == "--help" ]; then
    echo "run_build.sh - Generate build commands and run builds"
    echo
    echo "Supported options:"
    echo " --cmake: force cmake to run before building"
    echo " --no-ninja: don't run the ninja build; useful for running cmake only"
    echo ""
    echo "All other arguments are forwarded to the underlying ninja build"
    echo "CMake functionality can be customized by setting environment variables"
    exit 0
fi

SHOULD_CMAKE=0

cd "$(dirname "$0")" || exit 255

if [ -z "$BUILD_DIR" ]; then
    BUILD_DIR=build
fi

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
    SHOULD_CMAKE=1
fi

if [ "$1" == "--cmake" ]; then
    SHOULD_CMAKE=1
    shift
fi

if [ $SHOULD_CMAKE == 1 ]; then
    cmake -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DCPPINECONE_BUILD_TESTS=on -GNinja .
fi

if [ "$1" == "--no-ninja" ]; then
    exit 0
fi

cd "$BUILD_DIR" || exit 254
# shellcheck disable=SC2068
ninja $@

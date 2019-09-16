#!/bin/bash

set -e
#set -x

PREFIX="${PREFIX:-/opt/cross}"
ARCHS="x86_64 i686 aarch64 aarch64_be arm armeb mips mipsel mips64 mips64el sparc sparc64 powerpc powerpcle powerpc64 powerpc64le"

SCRIPT_DIR=$(dirname "${BASH_SOURCE[0]}")
SCRIPT_DIR=$(realpath "${SCRIPT_DIR}")

for TARGET_ARCH in ${ARCHS} ; do
    export CC="${PREFIX}/bin/${TARGET_ARCH}-xvi-gcc"
    export CXX="${PREFIX}/bin/${TARGET_ARCH}-xvi-g++"
    mkdir -p "_build/${TARGET_ARCH}"
    pushd "_build/${TARGET_ARCH}"
    cmake "${SCRIPT_DIR}" -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/xvi.toolchain.cmake" -DXVI_COMPILER=gcc -DXVI_ARCH=${TARGET_ARCH} $@
    popd
done

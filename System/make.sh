#!/bin/bash

#set -e

#ARCHS="x86_64 i686 aarch64 aarch64_be arm armeb mips mipsel mips64 mips64el sparc sparc64 powerpc powerpcle powerpc64 powerpc64le"
ARCHS="x86_64 i686 aarch64 arm mips mips64 sparc sparc64 powerpc powerpc64"

for TARGET_ARCH in ${ARCHS} ; do
    pushd _build/${TARGET_ARCH}
    cmake --build . $@
    popd
done

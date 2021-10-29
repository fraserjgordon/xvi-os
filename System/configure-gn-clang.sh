#!/bin/bash

TOOLCHAIN_ROOT=/opt/cross
#ARCHS='["x86", "x64", "arm", "arm64", "sparc", "sparc64", "mips", "mips64", "powerpc", "powerpc64"]'
ARCHS='["x86", "x64", "arm", "arm64", "sparc", "sparc64", "mips", "mips64", "powerpc", "powerpc64"]'

if [ ! -z "$1" ] ; then
    ARCHS="[\"$1\""
    shift
    while [ ! -z "$1" ] ; do
        ARCHS="${ARCHS}, \"$1\""
        shift
    done
    ARCHS="${ARCHS}]"
fi

gn gen --check out-clang --args="cross_toolchain_root=\"${TOOLCHAIN_ROOT}\" enable_archs=${ARCHS} use_clang=true" --export-compile-commands


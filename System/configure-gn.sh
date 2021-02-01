#!/bin/bash

TOOLCHAIN_ROOT=/opt/cross
#ARCHS='["x86", "x64", "arm", "sparc", "sparc64", "mips", "mips64", "powerpc", "powerpc64"]'
ARCHS='["x86", "x64", "arm", "mips", "mips64", "powerpc", "powerpc64"]'

gn gen --check out --args="cross_toolchain_root=\"${TOOLCHAIN_ROOT}\" enable_archs=${ARCHS}" --export-compile-commands


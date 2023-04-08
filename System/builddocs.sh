#!/bin/bash

set -e

DOXYGEN=${DOXYGEN:-/opt/cross/doxygen/bin/doxygen}

# Regenerate the build files to get compile-commands.json
ninja -C out-clang-native build.ninja

mkdir -p _docs/doxygen
sed Doxyfile.in \
    -e "s|@DOXYGEN_OUTPUT_DIR@|$(pwd)/_docs/doxygen|g" \
    -e "s|@DOXYGEN_INPUT_DIR@|$(pwd)|g" \
    > _docs/Doxyfile
LD_LIBRARY_PATH=/opt/cross/lib "${DOXYGEN}" _docs/Doxyfile
sphinx-build Docs _docs

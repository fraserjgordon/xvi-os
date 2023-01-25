#!/bin/bash

set -e

mkdir -p _docs/doxygen
sed Doxyfile.in \
    -e "s|@DOXYGEN_OUTPUT_DIR@|$(pwd)/_docs/doxygen|g" \
    -e "s|@DOXYGEN_INPUT_DIR@|$(pwd)|g" \
    > _docs/Doxyfile
doxygen _docs/Doxyfile
sphinx-build Docs _docs

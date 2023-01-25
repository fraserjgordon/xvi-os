#!/bin/bash

dir=${1:-out}

ninja -C "${dir}" test || exit $?

for exe in $(cat "${dir}/test_exes.list") ; do
    "./${dir}/${exe}"
done

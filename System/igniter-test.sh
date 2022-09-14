#!/bin/bash

set -e
set -o pipefail

clear && ninja -C out -j4 -k0 host_tools Igniter 2>&1 &&
objcopy -O binary --remove-section=discardme out/x86/x86/Stage1.{elf,bin} &&
objcopy --strip-all out/x86/x86/Stage2.x86{,.bin} &&
nm out/x86/x86/Stage1.elf | c++filt | cut -d' ' -f1,3- > out/x86/x86/Stage1.sym &&
nm out/x86/x86/Stage2.x86 | c++filt | cut -d' ' -f1,3- > out/x86/x86/Stage2.sym &&
cp igniter-clean.img igniter.img &&
mmd -i igniter.img ::/XVI &&
mmd -i igniter.img ::/XVI/Boot &&
mcopy -i igniter.img zero.dat ::/XVI/Boot/Bootloader.stage1 &&
mcopy -i igniter.img zero.dat ::/XVI/Boot/Bootloader.stage1.blocklist &&
mcopy -i igniter.img zero.dat ::/XVI/Boot/Bootloader.stage2.x86 &&
out/host/IgniterTool &&
/opt/bochs/bin/bochs -f bochsrc -q -rc <(echo -e "ldsym global \"out/x86/x86/Stage1.sym\"\nldsym global \"out/x86/x86/Stage2.sym\"\nc")

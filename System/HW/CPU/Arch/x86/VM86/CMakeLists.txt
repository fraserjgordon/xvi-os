###
### System::HW::CPU::Arch::X86::VM86
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.Arch.X86.VM86 LANGUAGES CXX)

xvi_library(System.HW.CPU.Arch.X86.VM86 VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.HW.CPU.Arch.X86.VM86+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
    Context.hh
    Emulator.hh
    Monitor.hh
    Task.hh
)

set(SOURCES
    Emulator.cc
    Monitor.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/CPU/Arch/x86/VM86/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/HW/CPU/Arch/x86/VM86/)

target_sources(System.HW.CPU.Arch.X86.VM86+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.HW.CPU.Arch.X86.VM86+All
    PUBLIC  System.HW.CPU.Arch.X86.ControlRegs
            System.HW.CPU.Arch.X86.Segmentation
    PRIVATE System.HW.CPU.Arch.X86.IO
)

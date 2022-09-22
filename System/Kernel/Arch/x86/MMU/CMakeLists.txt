###
### System::HW::CPU::Arch::X86::MMU
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.Arch.X86.MMU LANGUAGES CXX)

# Header-only library target.
xvi_library(System.HW.CPU.Arch.X86.MMU VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.HW.CPU.Arch.X86.MMU+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
)

set(SOURCES
    Cache.hh
    MTRR.hh
    PageTable.hh
    TLB.hh

    PageTable.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Kernel/Arch/x86/MMU/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Kernel/Arch/x86/MMU/)

target_sources(System.HW.CPU.Arch.X86.MMU+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.HW.CPU.Arch.X86.MMU+All
    PUBLIC System.HW.CPU.Arch.X86.ControlRegs
)

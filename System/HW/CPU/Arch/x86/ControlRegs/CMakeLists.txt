###
### System::HW::CPU::Arch::X86::ControlRegs
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.Arch.X86.ControlRegs LANGUAGES CXX)

# Header-only library target.
xvi_library(System.HW.CPU.Arch.X86.ControlRegs VERSION 0.1.0 INTERFACE KERNEL)

target_include_directories(System.HW.CPU.Arch.X86.ControlRegs
    INTERFACE Include/
)

set(HEADERS
    CR.hh
    EFLAGS.hh
    MSR.hh
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/CPU/Arch/x86/ControlRegs/)

target_sources(System.HW.CPU.Arch.X86.ControlRegs
    INTERFACE ${HEADERS}
)

target_link_libraries(System.HW.CPU.Arch.X86.ControlRegs
)

###
### System::HW::CPU::Arch::X86::IO
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.Arch.X86.IO LANGUAGES CXX)

# Header-only library target.
xvi_library(System.HW.CPU.Arch.X86.IO VERSION 0.1.0 INTERFACE KERNEL)

target_include_directories(System.HW.CPU.Arch.X86.IO
    INTERFACE   Include/
)

set(HEADERS
    IO.hh
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/CPU/Arch/x86/IO/)

target_sources(System.HW.CPU.Arch.X86.IO
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.HW.CPU.Arch.X86.IO
)

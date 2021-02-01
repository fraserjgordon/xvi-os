###
### System::Firmware::Arch::X86::MPS
###

cmake_minimum_required(VERSION 3.12)

project(System.Firmware.Arch.X86.MPS LANGUAGES CXX)

# Header-only library target.
xvi_library(System.Firmware.Arch.X86.MPS VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.Firmware.Arch.X86.MPS+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
    MPS.hh
)

set(SOURCES
    MPS.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Firmware/Arch/x86/MPS/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Firmware/Arch/x86/MPS/)

target_sources(System.Firmware.Arch.X86.MPS+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Firmware.Arch.X86.MPS+All
)

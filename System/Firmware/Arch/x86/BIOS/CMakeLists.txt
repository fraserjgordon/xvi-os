###
### System::Firmware::Arch::X86::BIOS
###

cmake_minimum_required(VERSION 3.12)

project(System.Firmware.Arch.X86.BIOS LANGUAGES CXX)

# Header-only library target.
xvi_library(System.Firmware.Arch.X86.BIOS VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.Firmware.Arch.X86.BIOS+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
    Call.hh
    DataArea.hh
    MemoryDetect.hh
)

set(SOURCES
    Call.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Firmware/Arch/x86/BIOS/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Firmware/Arch/x86/BIOS/)

target_sources(System.Firmware.Arch.X86.BIOS+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Firmware.Arch.X86.BIOS+All
)

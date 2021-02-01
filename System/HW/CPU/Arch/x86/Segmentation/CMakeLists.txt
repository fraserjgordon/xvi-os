###
### System::HW::CPU::Arch::X86::Segmentation
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.Arch.X86.Segmentation LANGUAGES CXX)

# Header-only library target.
xvi_library(System.HW.CPU.Arch.X86.Segmentation VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.HW.CPU.Arch.X86.Segmentation+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
    Descriptor.hh
    DescriptorTable.hh
    GDT.hh
    IDT.hh
    IVT.hh
    LDT.hh
    RealMode.hh
    Selector.hh
    TSS.hh
)

set(SOURCES
    Descriptor.cc
    DescriptorTable.cc
    GDT.cc
    IDT.cc
    IVT.cc
    LDT.cc
    Selector.cc
    TSS.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/CPU/Arch/x86/Segmentation/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/HW/CPU/Arch/x86/Segmentation/)

target_sources(System.HW.CPU.Arch.X86.Segmentation+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.HW.CPU.Arch.X86.Segmentation+All
    PUBLIC System.HW.CPU.Arch.X86.ControlRegs
)

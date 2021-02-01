###
### System::ABI::Atomic
###

cmake_minimum_required(VERSION 3.13)

include(FindDoxygen)

project(System.ABI.Atomic LANGUAGES CXX)

set(HEADERS
    Primitives.h
)

set(SOURCES_arm
    Arch/ARM/Primitives.cc
)

set(SOURCES_powerpc
    Arch/PowerPC/Primitives.cc
)

set(SOURCES_x86
    Arch/x86/Primitives.hh

    Arch/x86/Primitives.cc
    Arch/x86/Wait.cc
)

set(SOURCES
    AtomicSpinlock.hh

    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Atomic/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/Atomic/")


xvi_library(System.ABI.Atomic VERSION 1.0.0 STATIC SHARED KERNEL NO_STDLIB)

target_include_directories(System.ABI.Atomic+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.Atomic+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.Atomic+All
    PRIVATE System.ABI.String
            System.C++.LanguageSupport.Headers
            System.C++.TypeTraits
            #System.HW.CPU.CPUID
)

target_compile_definitions(System.ABI.Atomic+Shared PRIVATE __SYSTEM_ABI_ATOMIC_BUILD_SHARED)
target_compile_definitions(Kernel.ABI.Atomic PRIVATE ATOMIC_NO_RUNTIME_CHECKS)

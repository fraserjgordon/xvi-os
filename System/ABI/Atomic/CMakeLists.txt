###
### System::ABI::Atomic
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.Atomic LANGUAGES CXX)

set(HEADERS
)

set(SOURCES_arm
    Atomic-arm.cc
)

set(SOURCES_ppc
    Atomic-ppc.cc
)

set(SOURCES_x86
    Arch/x86/Primitives.hh

    Arch/x86/Primitives.cc
    #Arch/x86/Wait.cc
)

set(SOURCES
    AtomicSpinlock.hh

    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Atomic/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/Atomic/")


xvi_library(System.ABI.Atomic STATIC SHARED KERNEL)

target_include_directories(System.ABI.Atomic+
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.Atomic+
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.Atomic+
    PUBLIC  System.ABI.String
    PRIVATE System.C++.LanguageSupport
            System.C++.TypeTraits
            System.HW.CPU.CPUID
)

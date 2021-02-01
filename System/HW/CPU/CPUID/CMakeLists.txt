###
### System::HW::CPU::CPUID
###

cmake_minimum_required(VERSION 3.12)

project(System.HW.CPU.CPUID LANGUAGES CXX)

# Header-only library target.
xvi_library(System.HW.CPU.CPUID VERSION 0.1.0 INTERFACE KERNEL)

target_include_directories(System.HW.CPU.CPUID
    INTERFACE Include/
)

set(HEADERS_x86
    Arch/x86/CPUID.hh
)

set(HEADERS
    ${HEADERS_${XVI_ARCH}}
)

set(COMPILE_TEST_SOURCES_x86
    Arch/x86/TestCPUID.cc
)

set(COMPILE_TEST_SOURCES
    ${COMPILE_TEST_SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/CPU/CPUID/)
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/HW/CPU/CPUID/")

target_sources(System.HW.CPU.CPUID
    INTERFACE ${HEADERS}
)

target_link_libraries(System.HW.CPU.CPUID
)


##### Test library #####

if(OFF)

add_library(System.HW.CPU.CPUID_CompileTestLibrary STATIC)

target_sources(System.HW.CPU.CPUID_CompileTestLibrary
    PRIVATE ${COMPILE_TEST_SOURCES}
)

target_link_libraries(System.HW.CPU.CPUID_CompileTestLibrary
    System.HW.CPU.CPUID
)

endif()

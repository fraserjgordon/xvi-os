###
### System::ABI::TLS
###

cmake_minimum_required(VERSION 3.13)

project(System.ABI.TLS LANGUAGES CXX)

set(HEADERS
    TLS.hh
    Private/Config.hh

    Arch/x86/TLS.hh
)

set(SOURCES_arm
)

set(SOURCES_powerpc
)

set(SOURCES_x86
    Arch/x86/TLS.cc
)

set(SOURCES
    ImageInternal.hh
    Terminate.hh
    WeakMalloc.hh

    Image.cc

    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/TLS/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/TLS/")


xvi_library(System.ABI.TLS VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)

target_include_directories(System.ABI.TLS+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.TLS+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.TLS+All
    PRIVATE System.ABI.String
            System.C++.LanguageSupport.Headers
            System.C++.Utility.Headers
)

target_compile_definitions(System.ABI.TLS+Shared PRIVATE __SYSTEM_ABI_TLS_BUILD_SHARED)

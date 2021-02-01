###
### System::ABI::DWARF
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.Dwarf LANGUAGES CXX)

set(HEADERS_CFI
    Arch.hh
    CFI.hh
    Decode.hh
    Expression.hh
    FDE.hh
    LEB128.hh
    Registers_x86.hh
    RegistersX86_64.hh
)

set(HEADERS
    ${HEADERS_CFI}
)

set(SOURCES_arm
)

set(SOURCES_x86
)

set(SOURCES_CFI
    CFI.cc
    Decode.cc
    Expression.cc
    FDE.cc

    ${SOURCES_CFI_${XVI_ARCH}}
)

set(SOURCES
    ${SOURCES_CFI}
    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Dwarf/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/Dwarf/")
list(TRANSFORM HEADERS_CFI PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Dwarf/")
list(TRANSFORM SOURCES_CFI PREPEND "Source/System/ABI/Dwarf/")


xvi_library(System.ABI.Dwarf            VERSION 0.1.0 STATIC SHARED KERNEL)
xvi_library(System.ABI.Dwarf.CFI        VERSION 0.1.0 STATIC KERNEL NO_STDLIB)
xvi_library(System.ABI.Dwarf.Headers    VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.ABI.Dwarf+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_include_directories(System.ABI.Dwarf.CFI+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_sources(System.ABI.Dwarf+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_sources(System.ABI.Dwarf.CFI+All
    PUBLIC ${HEADERS_CFI}
    PRIVATE ${SOURCES_CFI}
)

target_compile_definitions(System.ABI.Dwarf+Shared
    PRIVATE __SYSTEM_ABI_DWARF_BUILD_SHARED
)

target_compile_definitions(System.ABI.Dwarf.CFI+All
    PRIVATE __SYSTEM_ABI_DWARF_CFI_ONLY
)

target_link_libraries(System.ABI.Dwarf+All
    PUBLIC  System.ABI.ExecContext.Headers
)

target_link_libraries(System.ABI.Dwarf.CFI+All
    PUBLIC  System.ABI.Dwarf.Headers
            System.ABI.ExecContext.Headers
            System.C++.LanguageSupport.Headers
            System.C++.Utility.Headers
    PRIVATE System.ABI.C++
            System.ABI.String
            System.C.LanguageSupport
)

target_include_directories(System.ABI.Dwarf.Headers
    INTERFACE   Include/
)

target_sources(System.ABI.Dwarf.Headers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.ABI.Dwarf.Headers
    INTERFACE   System.C++.LanguageSupport.Headers
                System.C++.Utility.Headers
)

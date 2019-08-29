###
### System::ABI::DWARF
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.Dwarf LANGUAGES CXX)

set(HEADERS
    Arch.hh
    CFI.hh
    Decode.hh
    Expression.hh
    FDE.hh
    LEB128.hh
    Registers_x86.hh
    RegistersX86_64.hh
)

set(SOURCES_arm
)

set(SOURCES_x86
)

set(SOURCES
    CFI.cc
    Expression.cc
    FDE.cc

    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Dwarf/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/Dwarf/")


add_library(System.ABI.Dwarf STATIC)

if (TRUE)
    target_compile_definitions(System.ABI.Dwarf
        PUBLIC __SYSTEM_ABI_DWARF_MINIMAL
    )
endif()

target_include_directories(System.ABI.Dwarf
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.Dwarf
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.Dwarf
    PUBLIC  System.C++.LanguageSupport
            System.C++.Utility
    PRIVATE System.ABI.ExecContext
            System.C.LanguageSupport
)

###
### System::C++::Locale
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Locale LANGUAGES CXX)

set(HEADERS
    CType.hh
    Locale.hh
    StdLib.hh
    UChar.hh

    Private/Config.hh
    Private/Namespace.hh
)

set(SOURCES
    Locale.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Locale/")
list(TRANSFORM SOURCES PREPEND "Source/System/C++/Locale/")


xvi_library(System.C++.Locale VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)
xvi_library(System.C++.Locale.Headers VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.Locale+All
    PUBLIC Include/
)

target_sources(System.C++.Locale+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.C++.Locale+All
    PUBLIC System.C++.Utility
)

target_include_directories(System.C++.Locale.Headers
    INTERFACE   Include/
)

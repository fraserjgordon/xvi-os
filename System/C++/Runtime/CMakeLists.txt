###
### System::C++::Runtime
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Runtime LANGUAGES CXX)

set(HEADERS
    Assert.hh
    Errno.hh
    Exception.hh
    SetJmp.hh
    Signal.hh
    StdLib.hh

    C/Errno.h

    Private/Config.hh
    Private/Namespace.hh
)

set(SOURCES
    Runtime.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Runtime/")
list(TRANSFORM SOURCES PREPEND "Source/System/C++/Runtime/")


xvi_library(System.C++.Runtime VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)

target_include_directories(System.C++.Runtime+All
    PUBLIC Include/
)

target_sources(System.C++.Runtime+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.C++.Runtime+All
    PUBLIC System.C++.LanguageSupport
           System.C++.Utility
)


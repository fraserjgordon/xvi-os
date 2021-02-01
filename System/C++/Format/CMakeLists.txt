###
### System::C++::Format
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Format LANGUAGES CXX)

set(HEADERS
    Format.hh

    Private/Config.hh
    Private/Namespace.hh
)

set(SOURCES
    Format.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Format/")
list(TRANSFORM SOURCES PREPEND "Source/System/C++/Format/")


xvi_library(System.C++.Format VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)
xvi_library(System.C++.Format.Headers   VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.Format+All
    PUBLIC Include/
)

target_sources(System.C++.Format+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.C++.Format+All
    PUBLIC  System.C++.LanguageSupport
            System.C++.Locale
            System.C++.Utility
    PRIVATE System.ABI.String
)

target_compile_definitions(System.C++.Format+SharedLibs
    PRIVATE __BUILD_SYSTEM_CXX_FORMAT_SHARED
)

target_include_directories(System.C++.Format.Headers
    INTERFACE   Include/
)

target_sources(System.C++.Format.Headers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.Format.Headers
    INTERFACE   System.C++.Locale.Headers
)

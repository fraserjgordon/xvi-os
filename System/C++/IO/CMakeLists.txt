###
### System.C++.IO
###

cmake_minimum_required(VERSION 3.12)

project(System.C++.IO LANGUAGES CXX)

set(HEADERS
    Private/Config.hh
)

set(SOURCES
)


list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/IO/")
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/C++/IO/")


xvi_library(System.C++.IO           VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)
xvi_library(System.C++.IO.Headers   VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

# Headers library
target_include_directories(System.C++.IO.Headers
    INTERFACE   Include/
)

target_sources(System.C++.IO.Headers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.IO.Headers
    INTERFACE   System.C++.LanguageSupport.Headers
)

# Main library
target_include_directories(System.C++.IO+All
    PUBLIC      Include/
    PRIVATE     Source/
)

target_sources(System.C++.IO+All
    PUBLIC      ${HEADERS}
    PRIVATE     ${SOURCES}
)

target_link_libraries(System.C++.IO+All
    PUBLIC      System.C++.IO.Headers
)

target_compile_definitions(System.C++.IO+Shared
    PRIVATE     __XVI_CXX_IO_BUILD_SHARED
)

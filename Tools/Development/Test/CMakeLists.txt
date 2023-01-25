cmake_minimum_required(VERSION 3.18)

project(Development.Test
    VERSION         0.1
    DESCRIPTION     "C++ testing framework"
    LANGUAGES       CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PUBLIC_HEADERS
    "Test.hh"
)

set(SOURCES
    "TestImpl.hh"
    "TestSuite.hh"

    "TestImpl.cc"
    "TestSuite.cc"
)

set(MAINLIB_SOURCES
    "MainLib.cc"
)

set(TEST_SOURCES
    "Macros.cc"
    "Test.cc"
)

list(TRANSFORM PUBLIC_HEADERS   PREPEND "Include/Tools/Development/Test/")
list(TRANSFORM SOURCES          PREPEND "Source/Tools/Development/Test/")
list(TRANSFORM MAINLIB_SOURCES  PREPEND "Source/Tools/Development/Test/")
list(TRANSFORM TEST_SOURCES     PREPEND "Test/Tools/Development/Test/")

add_library(Development.Test SHARED)

target_include_directories(Development.Test
    PUBLIC      "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

target_sources(Development.Test
    PUBLIC      ${PUBLIC_HEADERS}
    PRIVATE     ${SOURCES}
)


add_library(Development.Test.Main STATIC)

target_include_directories(Development.Test.Main
    PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

target_sources(Development.Test.Main
    PRIVATE     ${MAINLIB_SOURCES}
)


add_executable(Development.Test.TestExe)

target_include_directories(Development.Test.TestExe
    PRIVATE     "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

target_sources(Development.Test.TestExe 
    PRIVATE     ${TEST_SOURCES}
)

target_link_libraries(Development.Test.TestExe Development.Test)

###
### System::Filesystem::FAT
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.Filesystem.FAT LANGUAGES CXX)

set(HEADERS
    BPB.hh
    DirEnt.hh
    FAT.hh
    FAT12.hh
    Unaligned.hh
)

set(SOURCES
    BPB.cc
    DirEnt.cc
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
    SmokeTests-String.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Filesystem/FAT/")
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Filesystem/FAT/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/Filesystem/FAT/")


add_library(System.Filesystem.FAT STATIC)

target_include_directories(System.Filesystem.FAT
    PUBLIC  Include/
    PRIVATE Source/
)

target_sources(System.Filesystem.FAT
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Filesystem.FAT
    PUBLIC  System.C++.LanguageSupport
            System.C++.Utility
)

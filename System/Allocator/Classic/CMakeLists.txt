###
### System::Allocator::Classic
###

cmake_minimum_required(VERSION 3.13)

project(System.Allocator.Classic LANGUAGES CXX)

set(HEADERS
    Classic.hh

    Private/Config.hh
)

set(SOURCES
    Arena.hh
    Chunk.hh

    Arena.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Allocator/Classic/")
list(TRANSFORM SOURCES PREPEND "Source/System/Allocator/Classic/")


xvi_library(System.Allocator.Classic VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.Allocator.Classic+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.Allocator.Classic+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

#target_link_libraries(System.Allocator.Classic+All
#)

target_compile_definitions(System.Allocator.Classic+Shared
    PRIVATE __XVI_ALLOCATOR_CLASSIC_BUILD_SHARED
)

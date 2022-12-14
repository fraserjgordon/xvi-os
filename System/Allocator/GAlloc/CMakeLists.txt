###
### System::Allocator::GAlloc
###

cmake_minimum_required(VERSION 3.13)

include(FindDoxygen)

project(System.Allocator.GAlloc LANGUAGES CXX)

set(HEADERS
    Export.h
    GAlloc.h

    GAlloc
    GAlloc.hh
)

set(SOURCES
    Config.hh
    Imports.hh
    Locks.hh
    PageBlock.hh
    PageTable.hh
    Pool.hh
    PoolGroup.hh

    GAlloc.cc
    PageTable.cc
    Pool.cc
    PoolGroup.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Allocator/GAlloc/")
list(TRANSFORM SOURCES PREPEND "Source/System/Allocator/GAlloc/")

xvi_library(System.Allocator.GAlloc VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.Allocator.GAlloc+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.Allocator.GAlloc+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Allocator.GAlloc+All
)

###
### System::IPC::Sync
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.IPC.Sync LANGUAGES CXX)

set(HEADERS
)

set(SOURCES
    WaitOnAddress.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/IPC/Sync/")
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/IPC/Sync/")


xvi_library(System.IPC.Sync VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.IPC.Sync+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_sources(System.IPC.Sync+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.IPC.Sync+All
    PUBLIC
)

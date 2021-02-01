###
### System.Utility.Endian
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.Utility.Endian LANGUAGES CXX)

set(HEADERS
    Endian.hh
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Utility/Endian/")

xvi_library(System.Utility.Endian INTERFACE KERNEL)

target_include_directories(System.Utility.Endian
    INTERFACE   Include/
)

target_sources(System.Utility.Endian
    INTERFACE   ${HEADERS}
)

###
### System::C::BaseHeaders
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C.BaseHeaders LANGUAGES C)

set(HEADERS
    Limits.h
    Null.h
    OffsetOf.h
    Types.h
    VarArgs.h
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C/BaseHeaders/")

xvi_library(System.C.BaseHeaders VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C.BaseHeaders
    INTERFACE Include/
)

target_sources(System.C.BaseHeaders
    INTERFACE ${HEADERS}
)

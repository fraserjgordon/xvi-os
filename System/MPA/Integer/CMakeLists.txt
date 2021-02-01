###
### System.MPA.Integer
###

cmake_minimum_required(VERSION 3.12)

project(System.MPA.Integer)

set(HEADERS
    Types.hh
)

set(SOURCES
    Arch/Generic/add_1.cc
    Arch/Generic/add_mn.cc
    Arch/Generic/add_n.cc
    Arch/Generic/sub_1.cc
    Arch/Generic/sub_mn.cc
    Arch/Generic/sub_n.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/MPA/Integer/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/MPA/Integer/)

xvi_library(System.MPA.Integer   VERSION 0.1.0   STATIC SHARED KERNEL)

target_include_directories(System.MPA.Integer+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_compile_definitions(System.MPA.Integer+SharedLibs
    PRIVATE __BUILD_SYSTEM_MPA_INTEGER_SHARED
)

target_sources(System.MPA.Integer+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.MPA.Integer+All

)

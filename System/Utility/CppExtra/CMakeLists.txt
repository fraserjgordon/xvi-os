###
### System.Utility.CppExtra
###

cmake_minimum_required(VERSION 3.12)

project(System.Utility.CppExtra)

set(HEADERS
    RefcountedPtr.hh
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Utility/CppExtra/)

xvi_library(System.Utility.CppExtra VERSION 0.1.0 INTERFACE KERNEL)

target_include_directories(System.Utility.CppExtra
    INTERFACE   Include/
)

target_sources(System.Utility.CppExtra
    INTERFACE   ${HEADERS}    
)

target_link_libraries(System.Utility.CppExtra
    INTERFACE   System.Utility.Endian
)

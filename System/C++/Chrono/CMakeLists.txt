###
### System::C++::Chrono
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Chrono LANGUAGES CXX)

set(HEADERS
    Chrono.hh
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Chrono/")


xvi_library(System.C++.Chrono   VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.Chrono
    INTERFACE   Include/
)

target_sources(System.C++.Chrono
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.Chrono
    INTERFACE   System.ABI.C++
)

###
### System::Firmware::DeviceTree
###

cmake_minimum_required(VERSION 3.12)

project(System.Firmware.DeviceTree LANGUAGES CXX)

# Header-only library target.
xvi_library(System.Firmware.DeviceTree VERSION 0.1.0 STATIC SHARED KERNEL)

target_include_directories(System.Firmware.DeviceTree+All
    PUBLIC  Include/
    PRIVATE Source/
)

set(HEADERS
    FDT.hh
    Tree.hh
)

set(SOURCES
    Node.hh

    FDT.cc
    Node.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Firmware/DeviceTree/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Firmware/DeviceTree/)

target_sources(System.Firmware.DeviceTree+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Firmware.DeviceTree+All
    PUBLIC      System.Utility.Endian
)

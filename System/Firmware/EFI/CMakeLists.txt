###
### System.Firmware.EFI
###

cmake_minimum_required(VERSION 3.12)

project(System.Firmware.EFI)

set(HEADERS

)

set(SOURCES
    EFI.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Firmware/EFI/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Firmware/EFI/)

xvi_library(System.Firmware.EFI VERSION 0.1.0 KERNEL)

target_include_directories(System.Firmware.EFI+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_sources(System.Firmware.EFI+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

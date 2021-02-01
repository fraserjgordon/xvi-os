###
### System.HW.GPU.VGA
###


cmake_minimum_required(VERSION 3.12)


project(System.HW.GPU.VGA)

set(HEADERS
    AttributeController.hh
    CRTC.hh
    DAC.hh
    GraphicsController.hh
    Sequencer.hh
    VGA.hh
)

set(SOURCES
    CRTC.cc
    VGA.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/HW/GPU/VGA/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/HW/GPU/VGA/)

xvi_library(System.HW.GPU.VGA   VERSION 0.1.0   STATIC SHARED KERNEL)

target_include_directories(System.HW.GPU.VGA+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_sources(System.HW.GPU.VGA+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

if(XVI_ARCH STREQUAL x86)
    target_link_libraries(System.HW.GPU.VGA+All
        PRIVATE System.HW.CPU.Arch.X86.IO
    )
endif()

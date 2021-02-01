###
### System::Executable::Image::ELF
###

cmake_minimum_required(VERSION 3.13)

project(System.Executable.Image.ELF LANGUAGES CXX)

set(HEADERS
    Types.hh
)

set(SOURCES
    ELF.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Executable/Image/ELF/")
list(TRANSFORM SOURCES PREPEND "Source/System/Executable/Image/ELF/")

xvi_library(System.Executable.Image.ELF     VERSION 0.1.0   STATIC SHARED KERNEL)

target_include_directories(System.Executable.Image.ELF+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.Executable.Image.ELF+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Executable.Image.ELF+All
    PUBLIC      System.Utility.Endian
)

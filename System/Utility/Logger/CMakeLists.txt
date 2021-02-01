###
### System.Utility.Logger
###

cmake_minimum_required(VERSION 3.12)

project(System.Utility.Logger)

set(HEADERS
    Logger.hh
    Message.hh
    Processor.hh

    Private/Config.hh
)

set(SOURCES
    CircularBuffer.hh
    MessageInternal.hh
    ProcessorInternal.hh

    Logger.cc
    Message.cc
    Processor.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Utility/Logger/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Utility/Logger/)

xvi_library(System.Utility.Logger   VERSION 0.1.0   STATIC SHARED KERNEL)

target_include_directories(System.Utility.Logger+All
    PUBLIC  Include/
    PRIVATE Source/
)

target_compile_definitions(System.Utility.Logger+SharedLibs
    PRIVATE __BUILD_SYSTEM_UTILITY_LOGGER_SHARED
)

target_sources(System.Utility.Logger+All
    PUBLIC  ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.Utility.Logger+All
    PRIVATE System.Utility.CppExtra
)

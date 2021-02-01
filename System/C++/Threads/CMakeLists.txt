###
### System::C++::Threads
###

cmake_minimum_required(VERSION 3.12)

project(System.C++.Threads LANGUAGES CXX)

set(HEADERS
    Lock.hh
    Mutex.hh

    Private/Config.hh
)

set(SOURCES
    Mutex.cc
)


list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Threads/")
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/C++/Threads/")


xvi_library(System.C++.Threads          VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)
xvi_library(System.C++.Threads.Headers  VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

# Headers library
target_include_directories(System.C++.Threads.Headers
    INTERFACE   Include/
)

target_sources(System.C++.Threads.Headers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.Threads.Headers
    INTERFACE   System.C.LanguageSupport
                System.C++.Atomic
                System.C++.LanguageSupport.Headers
                System.C++.Utility.Headers
)

# Main library
target_include_directories(System.C++.Threads+All
    PUBLIC      Include/
    PRIVATE     Source/
)

target_sources(System.C++.Threads+All
    PUBLIC      ${HEADERS}
    PRIVATE     ${SOURCES}
)

target_link_libraries(System.C++.Threads+All
    PUBLIC      System.C++.Threads.Headers
    PRIVATE     System.ABI.C++
)

target_compile_definitions(System.C++.Threads+Shared
    PRIVATE     __XVI_CXX_THREADS_BUILD_SHARED
)

###
### System::ABI::String
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.String
    LANGUAGES   CXX
)

set(HEADERS
    String.hh
)

set(SOURCES
    StringImpl.hh

    memccpy.cc
    memchr.cc
    memcmp.cc
    memcpy.cc
    memmem.cc
    memmove.cc
    memrchr.cc
    memset.cc
    stpcpy.cc
    stpncpy.cc
    strcat.cc
    strchr.cc
    strcmp.cc
    strcpy.cc
    strlcat.cc
    strlcpy.cc
    strlen.cc
    strncat.cc
    strncmp.cc
    strncpy.cc
    strnlen.cc
    strrchr.cc
    strstr.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/String/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/String/")


xvi_library(System.ABI.String VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)

target_include_directories(System.ABI.String+All
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.String+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.String+All
    PUBLIC System.C++.LanguageSupport.Headers
)

target_compile_definitions(System.ABI.String+All
    PRIVATE __SYSTEM_ABI_STRING_BUILD
)

target_compile_definitions(System.ABI.String+Shared
    PRIVATE __SYSTEM_ABI_STRING_BUILD_SHARED
)

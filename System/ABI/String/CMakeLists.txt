###
### System::ABI::String
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.String
    LANGUAGES   CXX
    VERSION     0.1.0
)

set(HEADERS
)

set(SOURCES
    String.hh
    StringImpl.hh

    memccpy.cc
    memchr.cc
    memcmp.cc
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


xvi_library(System.ABI.String STATIC SHARED KERNEL)

target_include_directories(System.ABI.String+
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.String+
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.String+
    PRIVATE System.C++.LanguageSupport
)

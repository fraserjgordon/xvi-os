###
### System::Unicode::UTF
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.Unicode.UTF LANGUAGES CXX)

add_library(System.Unicode.UTF SHARED)

set(HEADERS
    Export.h
    UTF8.h
)

set(SOURCES
    UTF8Codec.hh
    UTF16Codec.hh

    UTF8.cc
    UTF8Codec.cc
)

set(TEST_SOURCES
    UTF8Codec.cc
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/Unicode/UTF/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Unicode/UTF/)
list(TRANSFORM TEST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Test/System/Unicode/UTF/)

target_sources(System.Unicode.UTF
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}    
)

target_include_directories(System.Unicode.UTF
    PUBLIC Include
    PRIVATE Source 
)

target_compile_definitions(System.Unicode.UTF
    PRIVATE System_ABI_ExecContext_BUILD
)

target_link_libraries(System.Unicode.UTF
)

if (DOXYGEN_FOUND)
    # Configure Doxygen macro expansion.
    set(DOXYGEN_ENABLE_PROCESSING YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
    set(DOXYGEN_PREDEFINED
        __cplusplus
        __x86_64__

        # Internal macros that don't need to be exposed in the docs.
        "__SYSTEM_UNICODE_UTF_EXPORT="
        "__SYSTEM_UNICODE_UTF_SYMBOL(x)="
    )

    # Generate the Doxygen docs for the main library.
    doxygen_add_docs(System.Unicode.UTF_Docs
        ${HEADERS}
        ${SOURCES}
    )
endif()


##### Test library #####

##### Test executable #####

include(GoogleTest)

enable_testing()

add_executable(System.Unicode.UTF_TestExe
    ${TEST_SOURCES}
)

target_include_directories(System.Unicode.UTF_TestExe
    PRIVATE Source Test
)

target_link_libraries(System.Unicode.UTF_TestExe
    PRIVATE System.Unicode.UTF
            gtest
            gtest_main
)

gtest_discover_tests(System.Unicode.UTF_TestExe
    TEST_PREFIX "System.Unicode.UTF."
)

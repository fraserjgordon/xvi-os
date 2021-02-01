###
### System::C++::Atomic
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Atomic LANGUAGES CXX)

set(HEADERS
    Atomic.hh

    Private/Config.hh
    Private/Namespace.hh
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Atomic/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/Atomic/")


xvi_library(System.C++.Atomic VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.Atomic
    INTERFACE Include/
)

target_sources(System.C++.Atomic
    INTERFACE ${HEADERS}
)

target_link_libraries(System.C++.Atomic
    INTERFACE System.ABI.Atomic
              System.C++.LanguageSupport.Headers
              System.C++.TypeTraits
)


##### Test library #####

if(OFF)

add_library(System.C++.Atomic_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.Atomic_CompileTestLibrary
    PRIVATE "__XVI_STD_NS=xvi_std"
)

target_link_libraries(System.C++.Atomic_CompileTestLibrary
    PRIVATE System.C++.Atomic
)

endif()

##### Test executable #####

if (XVI_GTEST)

    add_executable(System.C++.Atomic_TestExe
        ${TEST_SOURCES}
    )

    target_compile_definitions(System.C++.Atomic_TestExe
        PRIVATE "__XVI_STD_NS=xvi_std"
    )

    target_link_libraries(System.C++.Atomic_TestExe
        PRIVATE System.C++.Atomic
                gtest
                gtest_main
    )

    gtest_discover_tests(System.C++.Atomic_TestExe
        TEST_PREFIX "System.C++.Atomic"
    )

endif(XVI_GTEST)

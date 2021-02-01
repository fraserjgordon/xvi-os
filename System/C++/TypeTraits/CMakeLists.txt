###
### System::C++::TypeTraits
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.TypeTraits LANGUAGES CXX)

set(HEADERS
    Concepts.hh
    TypeTraits.hh

    Private/CompilerBuiltins.hh
    Private/Conditional.hh
    Private/Detect.hh
    Private/IntegralConstant.hh
    Private/Invoke.hh
    Private/Namespace.hh
    Private/Relations.hh
    Private/SimpleTraits.hh
    Private/Transforms.hh
    Private/VoidT.hh
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
)

list(TRANSFORM HEADERS PREPEND "Include/System/C++/TypeTraits/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/TypeTraits/")


xvi_library(System.C++.TypeTraits VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.TypeTraits
    INTERFACE Include/
)


##### Test library #####

if(OFF)

add_library(System.C++.TypeTraits_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.TypeTraits_CompileTestLibrary
    PRIVATE "__XVI_STD_NSs=xvi_std"
)

target_link_libraries(System.C++.TypeTraits_CompileTestLibrary
    PRIVATE System.C++.TypeTraits
)

endif()

##### Test executable #####

if (XVI_GTEST)

    add_executable(System.C++.TypeTraits_TestExe
        ${TEST_SOURCES}
    )

    target_compile_definitions(System.C++.TypeTraits_TestExe
        PRIVATE "__XVI_STD_NS=xvi_std"
    )

    target_link_libraries(System.C++.TypeTraits_TestExe
        PRIVATE System.C++.TypeTraits
                gtest
                gtest_main
    )

    gtest_discover_tests(System.C++.TypeTraits_TestExe
        TEST_PREFIX "System.C++.TypeTraits"
    )

endif(XVI_GTEST)

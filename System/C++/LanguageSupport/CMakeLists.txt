###
### System::C++::LanguageSupport
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.LanguageSupport LANGUAGES CXX)

set(HEADERS
    CLimits.hh
    Compare.hh
    Contract.hh
    Exception.hh
    InitializerList.hh
    Limits.hh
    New.hh
    StdArg.hh
    StdDef.hh
    StdInt.hh
    TypeInfo.hh
    Utility.hh

    Private/Namespace.hh
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/LanguageSupport/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/LanguageSupport/")


add_library(System.C++.LanguageSupport INTERFACE)

target_include_directories(System.C++.LanguageSupport
    INTERFACE Include/
)

target_sources(System.C++.LanguageSupport
    INTERFACE ${HEADERS}
)

target_link_libraries(System.C++.LanguageSupport
    INTERFACE System.C.BaseHeaders
              System.C++.TypeTraits
)


##### Test library #####

add_library(System.C++.LanguageSupport_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.LanguageSupport_CompileTestLibrary
    PRIVATE "__XVI_STD_NS=xvi_std"
)

target_link_libraries(System.C++.LanguageSupport_CompileTestLibrary
    PRIVATE System.C++.LanguageSupport
)

##### Test executable #####

if (XVI_GTEST)

    add_executable(System.C++.LanguageSupport_TestExe
        ${TEST_SOURCES}
    )

    target_compile_definitions(System.C++.LanguageSupport_TestExe
        PRIVATE "__XVI_STD_NS=xvi_std"
    )

    target_link_libraries(System.C++.LanguageSupport_TestExe
        PRIVATE System.C++.TypeTraits
                gtest
                gtest_main
    )

    gtest_discover_tests(System.C++.LanguageSupport_TestExe
        TEST_PREFIX "System.C++.LanguageSupport"
    )

endif(XVI_GTEST)

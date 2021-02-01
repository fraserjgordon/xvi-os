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

set(SOURCES
    Exception.cc
    New.cc
    Terminate.cc
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/LanguageSupport/")
list(TRANSFORM SOURCES PREPEND "Source/System/C++/LanguageSupport/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/LanguageSupport/")


xvi_library(System.C++.LanguageSupport VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)
xvi_library(System.C++.LanguageSupport.Headers VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.C++.LanguageSupport.Headers
    INTERFACE Include/
)

target_sources(System.C++.LanguageSupport.Headers
    INTERFACE ${HEADERS}
)

target_link_libraries(System.C++.LanguageSupport.Headers
    INTERFACE System.ABI.C++.RTTI.Headers
              System.C.BaseHeaders
              System.C++.TypeTraits
)

target_compile_definitions(System.C++.LanguageSupport+SharedLibs
    PRIVATE     __SYSTEM_CXX_LANGSUPPORT_BUILD_SHARED
)

target_compile_definitions(System.C++.LanguageSupport+StaticLibs
    PRIVATE     __SYSTEM_CXX_LANGUAGESUPPORT_BUILD_STATIC
)

target_sources(System.C++.LanguageSupport+All
    PUBLIC      ${HEADERS}
    PRIVATE     ${SOURCES}
)

target_link_libraries(System.C++.LanguageSupport+All
    PUBLIC      System.C++.LanguageSupport.Headers
                System.ABI.C++
    PRIVATE     System.C++.Atomic
)


##### Test library #####

if(OFF)

add_library(System.C++.LanguageSupport_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.LanguageSupport_CompileTestLibrary
    PRIVATE "__XVI_STD_NS=xvi_std"
)

target_link_libraries(System.C++.LanguageSupport_CompileTestLibrary
    PRIVATE System.C++.LanguageSupport
)

endif()

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

###
### System::C++::Utility
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.C++.Utility LANGUAGES CXX)

set(HEADERS
    Abs.hh
    Algorithm.hh
    Allocator.hh
    AllocatorTraits.hh
    Any.hh
    Array.hh
    Bind.hh
    Bit.hh
    Bitset.hh
    CharConv.hh
    CharTraits.hh
    Chrono.hh
    DefaultDelete.hh
    ErrorCodes.hh
    ExecutionPolicy.hh
    Function.hh
    FunctionalUtils.hh
    IntegerSequence.hh
    Invoke.hh
    IosFwd.hh
    Iterator.hh
    MBState.hh
    Memory.hh
    MemoryAlgorithms.hh
    MemoryResource.hh
    Numeric.hh
    Optional.hh
    Pair.hh
    PointerTraits.hh
    Ranges.hh
    Ratio.hh
    ReferenceWrapper.hh
    ScopedAllocator.hh
    Searcher.hh
    StdLib.hh
    SharedPtr.hh
    Span.hh
    StdExcept.hh
    String.hh
    StringFwd.hh
    StringView.hh
    Swap.hh
    SystemError.hh
    Tuple.hh
    TypeIndex.hh
    UniquePtr.hh
    Variant.hh

    Private/AddressOf.hh
    Private/Config.hh
    Private/InPlace.hh
    Private/Namespace.hh
    Private/TupleTraits.hh
)

set(STDLIB_SOURCES
    StdLib/voidptr_iter.hh
    StdLib/abs.cc
    StdLib/bsearch.cc
    StdLib/div.cc
    StdLib/qsort.cc
)

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
    SmokeTests-String.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Utility/")
list(TRANSFORM STDLIB_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/C++/Utility/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/Utility/")


# Main wrapper library.
xvi_library(System.C++.Utility VERSION 0.1.0 INTERFACE NO_STDLIB)

# Implementation libraries.
xvi_library(System.C++.Utility.Headers  VERSION 0.1.0 INTERFACE NO_STDLIB)
xvi_library(System.C++.Utility.Impl     VERSION 0.1.0 INTERFACE NO_STDLIB)
xvi_library(System.C++.Utility.StdLib   VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)


# Common settings for implementation libraries.
macro(cxx_utility_lib name)
    target_link_libraries(${name}+All PRIVATE System.C++.Utility.Impl PUBLIC System.C++.Utility.Headers)
    target_link_libraries(System.C++.Utility INTERFACE ${name})
endmacro()


# Headers library
target_include_directories(System.C++.Utility.Headers
    INTERFACE   Include/
)

target_sources(System.C++.Utility.Headers
    INTERFACE   ${HEADERS}
)

target_link_libraries(System.C++.Utility.Headers
    INTERFACE   System.C.LanguageSupport
                System.C++.Atomic
                System.C++.LanguageSupport.Headers
                System.C++.TypeTraits
)

target_link_libraries(System.C++.Utility
    INTERFACE   System.C++.LanguageSupport
)

# Impl library
target_include_directories(System.C++.Utility.Impl
    INTERFACE   Source/
)

target_link_libraries(System.C++.Utility.Impl
    INTERFACE   System.C++.Utility.Headers
)

# StdLib library
cxx_utility_lib(System.C++.Utility.StdLib)
target_sources(System.C++.Utility.StdLib+All
    PRIVATE     ${STDLIB_SOURCES}
)


##### Test library #####

if(OFF)

add_library(System.C++.Utility_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.Utility_CompileTestLibrary
    PRIVATE "__XVI_STD_NS=xvi_std"
)

target_link_libraries(System.C++.Utility_CompileTestLibrary
    PRIVATE System.C++.Utility
)

endif()

##### Test executable #####

if (XVI_GTEST)

    add_executable(System.C++.Utility_TestExe
        ${TEST_SOURCES}
    )

    target_compile_definitions(System.C++.Utility_TestExe
        PRIVATE "__XVI_STD_NS=xvi_std"
    )

    target_link_libraries(System.C++.Utility_TestExe
        PRIVATE System.C++.TypeTraits
                gtest
                gtest_main
    )

    gtest_discover_tests(System.C++.Utility_TestExe
        TEST_PREFIX "System.C++.Utility"
    )

endif(XVI_GTEST)

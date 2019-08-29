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
    SharedPtr.hh
    Span.hh
    StdExcept.hh
    String.hh
    StringFwd.hh
    StringView.hh
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

set(COMPILE_TEST_SOURCES
    SmokeTests.cc
    SmokeTests-String.cc
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/C++/Utility/")
list(TRANSFORM COMPILE_TEST_SOURCES PREPEND "Test/System/C++/Utility/")


add_library(System.C++.Utility INTERFACE)

target_include_directories(System.C++.Utility
    INTERFACE Include/
)

target_sources(System.C++.Utility
    INTERFACE ${HEADERS}
)

target_link_libraries(System.C++.Utility
    INTERFACE System.C.LanguageSupport
              System.C++.Atomic
              System.C++.LanguageSupport
              System.C++.TypeTraits
)


##### Test library #####

add_library(System.C++.Utility_CompileTestLibrary STATIC
    ${COMPILE_TEST_SOURCES}
)

target_compile_definitions(System.C++.Utility_CompileTestLibrary
    PRIVATE "__XVI_STD_NS=xvi_std"
)

target_link_libraries(System.C++.Utility_CompileTestLibrary
    PRIVATE System.C++.Utility
)

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

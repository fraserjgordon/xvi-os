###
### System::ABI::ExecContext
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.ExecContext LANGUAGES CXX ASM)

set(HEADERS_x86
    Arch/x86/Eflags.h
    Arch/x86/FloatingPoint.h
    Arch/x86/GeneralRegs.h
    Arch/x86/SSE.h
    Arch/x86/SysV_x64.h
    Arch/x86/SysV_x86.h
    Arch/x86/Xsave.h
)

set(HEADERS
    ${HEADERS_${XVI_ARCH}}
)

set(SOURCES_x86
    Arch/x86/FloatingPoint.cc
    Arch/x86/SysV_x64.cc
    Arch/x86/SysV_x64.S
    Arch/x86/SysV_x86.S
    Arch/x86/Xsave.cc
)

set(SOURCES
    ${SOURCES_${XVI_ARCH}}
)

set(TEST_SOURCES_x86
    Arch/x86/SysV_x64.cc
)

set(TEST_SOURCES
    ${TEST_SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/ExecContext/)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/ExecContext/)
list(TRANSFORM TEST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Test/System/ABI/ExecContext/)

add_library(System.ABI.ExecContext STATIC)

target_sources(System.ABI.ExecContext
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}    
)

target_include_directories(System.ABI.ExecContext
    PUBLIC Include
    PRIVATE Source 
)

target_compile_definitions(System.ABI.ExecContext
    PRIVATE System_ABI_ExecContext_BUILD
)

target_link_libraries(System.ABI.ExecContext
    PUBLIC  System.C.LanguageSupport
            System.C++.LanguageSupport
            System.C++.Utility
            System.HW.CPU.CPUID
    PRIVATE -Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/ExecContext/version.ld
)

set_target_properties(System.ABI.ExecContext
    PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/ExecContext/version.ld
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
        "__SYSTEM_ABI_EXECCONTEXT_EXPORT="
        "__SYSTEM_ABI_EXECCONTEXT_SYMBOL(x)="
    )

    # Generate the Doxygen docs for the main library.
    doxygen_add_docs(System.ABI.ExecContext_Docs
        ${HEADERS}
        ${SOURCES}
    )
endif()


##### Test library #####

##### Test executable #####

if (XVI_GTEST)

    include(GoogleTest)

    enable_testing()

    add_executable(System.ABI.ExecContext_TestExe
        ${TEST_SOURCES}
    )

    target_link_libraries(System.ABI.ExecContext_TestExe
        PRIVATE System.ABI.ExecContext
                gtest
                gtest_main
    )

    gtest_discover_tests(System.ABI.ExecContext_TestExe
        TEST_PREFIX "System.ABI.ExecContext."
    )

endif(XVI_GTEST)

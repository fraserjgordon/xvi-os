###
### System::ABI::ExecContext
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.ExecContext LANGUAGES CXX ASM)

set(HEADERS_x86
    Arch/x86/Eflags.hh
    Arch/x86/FloatingPoint.hh
    Arch/x86/GeneralRegs.hh
    Arch/x86/SSE.hh
    Arch/x86/SysV_x64.hh
    Arch/x86/SysV_x86.hh
    Arch/x86/Xsave.hh
)

set(HEADERS
    Private/Config.hh
    ${HEADERS_${XVI_ARCH}}
)

set(SOURCES_arm
    Arch/ARM/ARM.cc
)

set(SOURCES_powerpc
    Arch/PowerPC/ppc.cc
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

xvi_library(System.ABI.ExecContext.Headers  VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)
xvi_library(System.ABI.ExecContext          VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)

target_sources(System.ABI.ExecContext.Headers
    INTERFACE   ${HEADERS}
)

target_include_directories(System.ABI.ExecContext.Headers
    INTERFACE Include
)

target_sources(System.ABI.ExecContext+All
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}    
)

target_include_directories(System.ABI.ExecContext+All
    PUBLIC Include
    PRIVATE Source 
)

target_link_libraries(System.ABI.ExecContext+All
    PUBLIC  System.ABI.ExecContext.Headers
            System.HW.CPU.CPUID
    PRIVATE System.ABI.String
            System.C++.LanguageSupport.Headers
            System.C++.Utility.Headers
            -Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/ExecContext/version.ld
)

set_target_properties(System.ABI.ExecContext+All
    PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/ExecContext/version.ld
)

target_compile_definitions(System.ABI.ExecContext+Shared PRIVATE SYSTEM_ABI_EXECCONTEXT_BUILD_SHARED)

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

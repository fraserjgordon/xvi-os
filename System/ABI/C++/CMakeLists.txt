###
### System::ABI::C++
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.C++
    LANGUAGES   CXX
)

set(HEADERS
    Demangle.hh
    DynamicCast.hh
    Exception.hh
    Guard.hh
    TypeInfo.hh
    Unwinder.hh
    VecCtor.hh
    Virtual.hh

    Private/Config.hh
)

set(SOURCES_AtExit
    AtExit/AtExit.cc
)

set(SOURCES_Demangle
    Demangle/Demangle.cc
)

set(SOURCES_EhPersonality
    EhPersonality/GxxPersonality_v0.cc
)

set(SOURCES_Exception
    Exception/Exception.cc
    Exception/Terminate.cc
    Exception/Unexpected.cc
)

set(SOURCES_Guard
    Guard/Guard.cc
)

set(SOURCES_NewDelete
    NewDelete/NewDelete.cc
)

set(SOURCES_PriorityInit
    PriorityInit/PriorityInit.cc
)

set(SOURCES_RTTI
    RTTI/DynamicCast.cc
    RTTI/TypeInfo.cc
)

set(SOURCES_Unwind
    Unwind/Unwinder.hh

    Unwind/DwarfUnwinder.cc
    Unwind/NullUnwinder.cc
    Unwind/Unwind.cc
    Unwind/Unwinder.cc
)

set(SOURCES_VecCtor
    VecCtor/VecCtor.cc
)

set(SOURCES_Virtual
    Virtual/DeletedVirtual.cc
    Virtual/PureVirtual.cc
)


list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/C++/")


xvi_library(System.ABI.C++          VERSION 0.1.0 STATIC SHARED KERNEL NO_STDLIB)

target_compile_definitions(System.ABI.C+++Shared
    PRIVATE     __SYSTEM_ABI_CXX_BUILD_SHARED
)

target_link_options(System.ABI.C+++Shared
    PRIVATE     -Wl,--dynamic-list-cpp-typeinfo
)

macro(system_abi_cxx_module NAME)
    xvi_library(System.ABI.C++.${NAME} INTERFACE ${ARGN} NO_STDLIB)
    string(TOUPPER ${NAME} NAME_UPPER)
    list(TRANSFORM SOURCES_${NAME} PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Source/System/ABI/C++/")
    target_link_libraries(System.ABI.C+++All
        PRIVATE     System.ABI.C++.${NAME}
    )
    target_link_libraries(System.ABI.C++.${NAME}
        INTERFACE   System.C++.LanguageSupport.Headers
                    System.C++.Utility.Headers
    )
    target_include_directories(System.ABI.C++.${NAME}
        INTERFACE   Include/ Source/
    )
    target_sources(System.ABI.C++.${NAME}
        INTERFACE   ${SOURCES_${NAME}}
    )
endmacro()

system_abi_cxx_module(AtExit        VERSION 0.1.0)
system_abi_cxx_module(Demangle      VERSION 0.1.0 KERNEL)
system_abi_cxx_module(EhPersonality VERSION 0.1.0 KERNEL)
system_abi_cxx_module(Exception     VERSION 0.1.0 KERNEL)
system_abi_cxx_module(Guard         VERSION 0.1.0)
system_abi_cxx_module(NewDelete     VERSION 0.1.0 KERNEL)
system_abi_cxx_module(PriorityInit  VERSION 0.1.0 KERNEL)
system_abi_cxx_module(RTTI          VERSION 0.1.0 KERNEL)
system_abi_cxx_module(Unwind        VERSION 0.1.0 KERNEL)
system_abi_cxx_module(VecCtor       VERSION 0.1.0 KERNEL)
system_abi_cxx_module(Virtual       VERSION 0.1.0 KERNEL)

target_link_libraries(System.ABI.C++.Demangle
    INTERFACE   System.ABI.String
                System.C++.Format.Headers
)

target_link_libraries(System.ABI.C++.EhPersonality
    INTERFACE   System.ABI.C++.Unwind
                System.ABI.Dwarf.Headers
)

target_link_libraries(System.ABI.C++.Exception
    INTERFACE   System.ABI.C++.EhPersonality
                System.ABI.C++.Unwind
                System.ABI.C++.RTTI
                System.ABI.TLS
                System.C++.Atomic
)

target_link_libraries(System.ABI.C++.Unwind
    INTERFACE   System.ABI.C++.RTTI
                System.ABI.C++.Virtual
                System.ABI.Dwarf.CFI
                System.ABI.ExecContext
                System.C++.Containers
)

# Needed to resolve a circular dependency with System.C++.LanguageSupport.Headers
xvi_library(System.ABI.C++.RTTI.Headers VERSION 0.1.0 INTERFACE KERNEL NO_STDLIB)

target_include_directories(System.ABI.C++.RTTI.Headers
    INTERFACE   Include/
)

# For ARM targets, the unwinder library needs to use a different ABI.
if (FALSE)
    target_compile_definitions(System.ABI.C++.Unwind INTERFACE UNWIND_AEABI)
endif()

target_link_libraries(System.ABI.C++
    INTERFACE   System.ABI.Integer
                System.ABI.String
                System.ABI.TLS
)


### FIX ME ###
add_library(System.ABI.C++.Unwind.Test SHARED)
target_sources(System.ABI.C++.Unwind.Test PRIVATE Test/test.cc)
target_compile_options(System.ABI.C++.Unwind.Test PRIVATE 
    -std=c++20
    -ffunction-sections
    -fdata-sections
)
target_link_libraries(System.ABI.C++.Unwind.Test PRIVATE
    -Wl,--start-group
    System.ABI.C+++Static
    System.ABI.Dwarf.CFI+Static
    System.ABI.ExecContext+Static
    System.ABI.String+Static
    System.C++.StdLib.Headers
    System.C++.Containers
    System.C++.LanguageSupport+Static
    System.C++.Format+Static
    System.Utility.Logger+Static
    -Wl,--end-group
    -zmax-page-size=4096
    -zcommon-page-size=4096
    -Wl,--gc-sections
)
target_link_libraries(System.ABI.C++.Unwind.Test PRIVATE -nostdlib)

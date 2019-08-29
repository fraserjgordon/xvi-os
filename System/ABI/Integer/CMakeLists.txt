###
### System::ABI::Integer
###

cmake_minimum_required(VERSION 3.12)

include(FindDoxygen)

project(System.ABI.Integer LANGUAGES CXX)

set(HEADERS
)

set(SOURCES_arm
    Arch/arm/BitOps.hh
    Arch/arm/Capabilities.hh
)

set(SOURCES_x86
    Arch/x86/BitOps.hh
    Arch/x86/Capabilities.hh
)

set(SOURCES
    Arithmetic.hh
    ArithmeticOpsImpl.hh
    BitOps.hh
    BitOpsImpl.hh
    Export.hh
    Types.hh

    ashldi.cc
    ashlsi.cc
    ashlti.cc
    ashrdi.cc
    ashrsi.cc
    ashrti.cc
    bswapdi.cc
    bswapsi.cc
    clzdi.cc
    clzsi.cc
    clzti.cc
    cmpdi.cc
    cmpti.cc
    divdi.cc
    divsi.cc
    divti.cc
    divmoddi.cc
    divmodsi.cc
    divmodti.cc
    ffsdi.cc
    ffssi.cc
    ffsti.cc
    lshrdi.cc
    lshrsi.cc
    lshrti.cc
    moddi.cc
    modsi.cc
    modti.cc
    muldi.cc
    mulsi.cc
    multi.cc
    negdi.cc
    negti.cc
    paritydi.cc
    paritysi.cc
    parityti.cc
    popcountdi.cc
    popcountsi.cc
    popcountti.cc
    ucmpdi.cc
    ucmpti.cc
    udivdi.cc
    udivsi.cc
    udivti.cc
    udivmoddi.cc
    udivmodsi.cc
    udivmodti.cc
    umoddi.cc
    umodsi.cc
    umodti.cc

    ${SOURCES_${XVI_ARCH}}
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/Include/System/ABI/Integer/")
list(TRANSFORM SOURCES PREPEND "Source/System/ABI/Integer/")


xvi_library(System.ABI.Integer STATIC SHARED KERNEL)

target_include_directories(System.ABI.Integer+
    PUBLIC Include/
    PRIVATE Source/
)

target_sources(System.ABI.Integer+
    PUBLIC ${HEADERS}
    PRIVATE ${SOURCES}
)

target_link_libraries(System.ABI.Integer+
    PRIVATE System.C++.LanguageSupport
)

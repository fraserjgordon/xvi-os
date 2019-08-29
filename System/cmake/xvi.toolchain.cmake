# Include the CMake modules for XVI.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_SYSTEM_NAME XVI)

#set(CMAKE_SYSROOT ...)
set(CMAKE_STAGING_PREFIX ${CMAKE_SOURCE_DIR}/.xvi.stage)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Path to the XVI compilers.
if (NOT DEFINED XVI_TOOLCHAIN_DIR)
    set(XVI_TOOLCHAIN_DIR /opt/cross/bin/)
endif()

# Don't try to build an executable to test the toolchain.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Which compiler set are we using?
#
# If none is explicitly requested, default to clang.
if ("${XVI_COMPILER}" STREQUAL "gcc")
    set(XVI_GCC ON)

    # The triple needs to be explicitly specified when using GCC.
    if (NOT DEFINED XVI_ARCH)
        message(FATAL_ERROR "XVI_ARCH must be defined when using GCC.")
    endif()
    if (NOT DEFINED XVI_TRIPLE)
        set(XVI_TRIPLE ${XVI_ARCH}-xvi)
    endif()
elseif("${XVI_COMPILER}" STREQUAL "clang" OR NOT DEFINED XVI_COMPILER)
    set(XVI_CLANG ON)

    # Set the triple to pass to Clang (if specified). Otherwise, default to the host triple.
    if (DEFINED XVI_ARCH)
        set(XVI_TRIPLE ${XVI_ARCH}-xvi)
        set(CMAKE_C_COMPILER_TARGET ${XVI_TRIPLE})
        set(CMAKE_CXX_COMPILER_TARGET ${XVI_TRIPLE})
    endif()

    # Set the path to the compilers.
    set(CMAKE_C_COMPILER ${XVI_TOOLCHAIN_DIR}clang)
    set(CMAKE_CXX_COMPILER ${XVI_TOOLCHAIN_DIR}clang++)
    set(CMAKE_ASM_COMPILER ${CMAKE_TOOLCHAIN_DIR}clang)
else()
    message(FATAL_ERROR "Unrecognised setting for XVI_COMPILER \"${XVI_COMPILER}\". Set to \"gcc\" or \"clang\".")
endif()

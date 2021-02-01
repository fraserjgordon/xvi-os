# Assert that this is the XVI operating system.
set(XVI ON)

# Converts a CMake processor type to an XVI architecture.
function(xvi_arch_from_cmake_processor ARCH SUBARCH PROC)
    if ("${PROC}" MATCHES "^i[3-6]86$" OR "${PROC}" STREQUAL "x86_64")
        set(${ARCH} x86 PARENT_SCOPE)
        set(${SUBARCH} ${PROC} PARENT_SCOPE)
    elseif("${PROC}" MATCHES "powerpc(64)?(le)?")
        set(${ARCH} powerpc PARENT_SCOPE)
        set(${SUBARCH} ${PROC} PARENT_SCOPE)
    elseif("${PROC}" MATCHES "aarch64(_be)?")
        set(${ARCH} arm PARENT_SCOPE)
    elseif("${PROC}" MATCHES "arm.*")
        SET(${ARCH} arm PARENT_SCOPE)
    elseif("${PROC}" MATCHES "mips.*")
        SET(${ARCH} mips PARENT_SCOPE)
    elseif("${PROC}" MATCHES "sparc.*")
        SET(${ARCH} sparc PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown architecture \"${PROC}\".")
    endif()
endfunction()

macro(xvi_arch_from_xvi_target ARCH SUBARCH TARGET)
    xvi_arch_from_cmake_processor(${ARCH} ${SUBARCH} ${TARGET})
endmacro()

# Processor type configuration.
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
    if(NOT "${XVI_TARGET}" STREQUAL "")
        xvi_arch_from_xvi_target(XVI_ARCH XVI_SUBARCH "${XVI_TARGET}")
        set(CMAKE_SYSTEM_PROCESSOR "${XVI_ARCH}")
    elseif("${XVI_ARCH}" STREQUAL "")
        # Guess the CPU type based on the host.
        xvi_arch_from_cmake_processor(XVI_ARCH XVI_SUBARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
        set(CMAKE_SYSTEM_PROCESSOR "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    else()
        set(CMAKE_SYSTEM_PROCESSOR "${XVI_ARCH}")
    endif()
endif()

# Include the architecture-specific settings.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/XVI.${XVI_ARCH}.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/XVI.${XVI_ARCH}.cmake")
endif()

# Set the emulator to use (if cross-compiling).
if (FALSE AND CMAKE_CROSSCOMPILING)
    set(CMAKE_CROSSCOMPILING_EMULATOR qemu-${XVI_QEMU_ARCH})
endif()

set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP ":")
set(CMAKE_SHARED_LIBRARY_RPATH_LINK_C_FLAG "-Wl,-rpath-link,")
set(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-Wl,-soname,")
set(CMAKE_EXE_EXPORTS_C_FLAG "-Wl,--export-dynamic")

foreach(type SHARED_LIBRARY SHARED_MODULE EXE)
  set(CMAKE_${type}_LINK_STATIC_C_FLAGS "-Wl,-Bstatic")
  set(CMAKE_${type}_LINK_DYNAMIC_C_FLAGS "-Wl,-Bdynamic")
endforeach()

# No shared library prefix on XVI.
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

# Nor for static libraries.
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

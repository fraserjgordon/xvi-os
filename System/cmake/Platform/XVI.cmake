# Assert that this is the XVI operating system.
set(XVI ON)

# Converts a CMake processor type to an XVI architecture.
function(xvi_arch_from_cmake_processor ARCH SUBARCH PROC)
    if ("${PROC}" MATCHES "^i[3-6]86$" OR "${PROC}" STREQUAL "x86_64")
        set(${ARCH} x86 PARENT_SCOPE)
        set(${SUBARCH} ${PROC} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown architecture \"${PROC}\".")
    endif()
endfunction()

# Processor type configuration.
if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
    # Guess the CPU type based on the host.
    xvi_arch_from_cmake_processor(XVI_ARCH XVI_SUBARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    set(CMAKE_SYSTEM_PROCESSOR "${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

# Include the architecture-specific settings.
include("${CMAKE_CURRENT_LIST_DIR}/XVI.${XVI_ARCH}.cmake")

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

# Symbols are hidden by default to minimise dynamic linking tables.
add_compile_options(
    -fvisibility=hidden
)

# Set the QEMU variant to use if cross-compiling.
if (${XVI_SUBARCH} MATCHES "^i[3-6]86")
    set(XVI_QEMU_ARCH "i386")
elseif(${XVI_SUBARCH} STREQUAL "x86_64")
    set(XVI_QEMU_ARCH "x86_64")
endif()

add_compile_options(
#    -march=native
)

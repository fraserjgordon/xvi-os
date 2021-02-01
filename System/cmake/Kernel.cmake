target_compile_definitions(__XviKernel
    INTERFACE "-DXVI_KERNEL"
)

# Assert that we're compiling in a freestanding (not hosted) environment.
#
# This doesn't really change much but doesn't do any harm, either.
target_compile_options(__XviKernel
    INTERFACE   -ffreestanding
)


# x86-specific options for kernel builds.
if (XVI_ARCH STREQUAL "x86")

    target_compile_options(__XviKernel
        INTERFACE   -mno-sse
    )

    if (XVI_SUBARCH STREQUAL "x86_64")
        target_compile_options(__XviKernel
            INTERFACE   -mcmodel=kernel -fno-pic
        )
    endif()

endif()

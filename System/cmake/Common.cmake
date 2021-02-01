# Language standards required:
#   C:      C11
#   C++:    C++20
#set_target_properties(__Xvi
#    PROPERTIES      INTERFACE_COMPILE_FEATURES  "c_std_11;cxx_std_20"
#)
target_compile_options(__Xvi
    INTERFACE   $<$<COMPILE_LANGUAGE:C>:-std=c11>
                $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a>
)

# Enable (nearly) all warnings for C and C++.
set(__XVI_WARNINGS_COMMON
    -Wall
    -Wextra
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wshadow=local
    -Wformat=2
    #-Wconversion
    -Wsign-conversion
)
set(__XVI_WARNINGS_C
    -Wbad-function-cast
    -Wc++-compat
    -Wmissing-prototypes
)
set(__XVI_WARNINGS_CXX
    #-Wuseless-cast
    -Wimplicit-fallthrough=5
    -Wsized-deallocation
    -Wmissing-declarations
)
target_compile_options(__Xvi
    INTERFACE   $<$<COMPILE_LANGUAGE:C>:${__XVI_WARNINGS_COMMON} ${__XVI_WARNINGS_C}>
                $<$<COMPILE_LANGUAGE:CXX>:${__XVI_WARNINGS_COMMON} ${__XVI_WARNINGS_CXX}>
                $<$<COMPILE_LANGUAGE:ASM>:-Wall>
)

# Enforce some coding standards via warnings-as-errors.
set(__XVI_WERROR_COMMON
    -Werror=init-self
    -Werror=implicit-fallthrough
    -Werror=trampolines
    -Werror=pointer-arith
    -Werror=write-strings
    -Werror=date-time
    -Werror=alloca
)
set(__XVI_WERROR_C
    -Werror=jump-misses-init
    -Werror=strict-prototypes
    -Werror=old-style-declaration
    -Werror=old-style-definition
)
set(__XVI_WERROR_CXX
    -Werror=old-style-cast
    -Werror=zero-as-null-pointer-constant
)
target_compile_options(__Xvi
    INTERFACE   $<$<COMPILE_LANGUAGE:C>:${__XVI_WERROR_COMMON} ${__XVI_WERROR_C}>
                $<$<COMPILE_LANGUAGE:CXX>:${__XVI_WERROR_COMMON} ${__XVI_WERROR_CXX}>
)

# Force symbols to be hidden unless declared otherwise.
target_compile_options(__Xvi
    INTERFACE   $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:-fvisibility=hidden>
)

# Common ABI/codegen options:
#   - elide the PLT when not needed (makes PIC slightly more efficient)
#   - enable stack protection
#   - probe the stack when expanding to prevent "Stack Clash" bugs
#   - enable split stacks
#   - each data object should be in its own section
#   - each function should be in its own section
target_compile_options(__Xvi
    INTERFACE   -fno-plt
                #-fstack-protector-strong
                $<$<CXX_COMPILER_ID:GNU>:-fstack-clash-protection>
                #-fsplit-stack
                -fdata-sections
                -ffunction-sections
)

# Debugging information configuration.
target_compile_options(__Xvi
    INTERFACE   -gdwarf-5 -g3
)

# Common linking options:
#   - link internal references directly
#   - don't use lazy symbol binding
#   - stack should not be executable
#   - relocations should be read-only after loading
#   - code should not be in the same segment as non-code
#   - forbid relocations in code sections
#   - only link libraries that supply an imported symbol
#   - we have a linker that understands modern dynamic tags
#   - we want an .eh_frame_hdr section
#   - use the improved GNU hashing for symbols
#   - add a build ID to the output
#
# TODO:
#    - semantics of -Bgroup; is it useful?
#
# UPDATE:
#   - these are now all set in the GCC specs.

# Work around a GCC crashing bug.
#
# These parameters override the default GCC garbage collector settings.
target_compile_options(__Xvi
    INTERFACE $<$<CXX_COMPILER_ID:GNU>:--param=ggc-min-expand=100 --param=ggc-min-heapsize=131072>
)

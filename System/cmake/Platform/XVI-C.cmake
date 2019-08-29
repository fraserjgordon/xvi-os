# May get included multiple times so guard against that.
if (NOT DEFINED XVI_C_INCLUDED)
set(XVI_C_INCLUDED TRUE)

# XVI doesn't support pre-C11 C compilers.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable most (not quite all) warnings by default.
add_compile_options(
    -Wall -Wextra
)

# Force hidden symbol visibility unless told otherwise.
add_compile_options(
    #-fvisibility=hidden
)

# Link symbolically unless told otherwise.
add_link_options(
    -Wl,-Bsymbolic
)

endif()

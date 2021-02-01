# REMOVE ME
target_compile_options(__XviShared
    INTERFACE -nostdinc -nostdlib
)

# REMOVE ME
target_link_options(__XviShared
    INTERFACE -nostdlib
)

# Ensure shared libraries have all symbols defined.
target_link_options(__XviShared
    INTERFACE -Wl,-z,defs
)

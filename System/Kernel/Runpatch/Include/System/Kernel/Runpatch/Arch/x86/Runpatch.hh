#ifndef __SYSTEM_KERNEL_RUNPATCH_ARCH_X86_RUNPATCH_H
#define __SYSTEM_KERNEL_RUNPATCH_ARCH_X86_RUNPATCH_H


#define RUNPATCH_STATIC_IF(key, then, default) \
    asm goto inline \
    ( \
        ".Lrpsi%=_jmp: \n\t" \
        "jmp.d32 %l[" #then "] \n\t" \
        "\n\t" \
        ".pushsection runpatch.static_if.keys, \"aMS\", @progbits, 1 \n\t" \
            ".Lrpsi%=_key: \n\t" \
            ".asciz " #key " \n\t" \
        ".section runpatch.static_if.destinations, \"a\", @progbits \n\t" \
            ".Lrpsi%=_dests: \n\t" \
            ".long $%l[" #then "] \n\t" \
            ".long $%l[" #default "] \n\t" \
        ".section runpatch.static_if.control, \"a\", @progbits \n\t" \
            ".long $.Lrpsi%=_key \n\t" \
            ".long $.Lrpsi%=_jmp \n\t" \
            ".long $.Lrpsi%=_dests \n\t" \
        ".popsection \n\t" \
        "\n\t" \
        : \
        : \
        : \
        : then, default \
    )


asm
(
R"(

.altmacro

.macro runpatch_emit_alternative_impl idx:req code:req

    # Write this alternative's code to a separate section.
    .pushsection runpatch.alternatives.text, "a", @progbits
        .Lrp\idx\()_alt = .
            \code
        .Lrp\idx\()_alt_end = .
        .Lrp\idx\()_alt_size = .Lrp\idx\()_alt_end - .Lrp\idx\()_alt

        # Calculate the maximum alternative size so far.
        .if .Lrp\idx\()_alt_size > .Lrp\idx\()_alt_max
            .Lrp\idx\()_alt_max = .Lrp\idx\()_alt_size
        .endif
    .popsection

    # Record this alternative's location and size.
    .pushsection runpatch.alternatives.info, "a", @progbits
        .long   .Lrp\idx\()_alt
        .long   .Lrp\idx\()_alt_size
    .popsection
.endm

.macro runpatch_emit_alternative idx:req code:req
    # If this is the first alternative, set up our book-keeping variables.
    .ifnotdef .Lrp\idx\()_alt_info
        .pushsection runpatch.alternatives.info, "a", @progbits
            .Lrp\idx\()_alt_info = .
        .popsection

        .Lrp\idx\()_alt_count = 0
        .Lrp\idx\()_alt_max = 0
    .endif

    runpatch_emit_alternative_impl  \idx "\code"

    .Lrp\idx\()_alt_count = .Lrp\idx\()_alt_count + 1
.endm

.macro runpatch_emit_default idx:req code:req
    # Emit the default's code as if it was an alternative, in case we need to restore it later.
    .pushsection runpatch.alternatives.text, "a", @progbits
        .Lrp\idx\()_default:
            \code
        .Lrp\idx\()_default_end = .
        .Lrp\idx\()_default_size = .Lrp\idx\()_default_end - .Lrp\idx\()_default
    .popsection

    # Emit the default's code again, this time in the proper place.
    .Lrp\idx\()_location:
        \code
    .Lrp\idx\()_location_mid = .
    .Lrp\idx\()_location_mid_size = .Lrp\idx\()_location_mid - .Lrp\idx\()_location

    # If we've seen alternatives that are longer than the default's code, pad with nops until we have enough space for
    # any of the alternatives to be written here.
    .if .Lrp\idx\()_alt_max > .Lrp\idx\()_location_mid_size
        .nop .Lrp\idx\()_alt_max - .Lrp\idx\()_location_mid_size
    .endif

    .Lrp\idx\()_location_end = .
    .Lrp\idx\()_location_size = .Lrp\idx\()_location_end - .Lrp\idx\()_location
.endm

.macro runpatch_start idx:req key:req
    # Emit the key string identifying this patch.
    .pushsection runpatch.keys, "aSM", @progbits, 1
        .Lrp\idx\()_key: .asciz "\key"
    .popsection
.endm

.macro runpatch_finish idx:req
    # Write out the control information for the patch and all alternatives.
    .pushsection runpatch.control, "aR", @progbits
        .long   .Lrp\idx\()_key
        .long   .Lrp\idx\()_location
        .long   .Lrp\idx\()_location_size
        .long   .Lrp\idx\()_alt_info
        .long   .Lrp\idx\()_alt_count
        .long   .Lrp\idx\()_default
        .long   .Lrp\idx\()_default_size
        .long   0
    .popsection
.endm

.noaltmacro

)"
);


#define RUNPATCH_START(key) \
    "runpatch_start %=, " #key " \n\t"

#define RUNPATCH_DEFINE_ALTERNATIVE(code) \
    "runpatch_emit_alternative %=, " #code " \n\t"

// Because the code strings cannot be empty, define this convenience macro.
#define RUNPATCH_DEFINE_ALTERNATIVE_NOP() \
    RUNPATCH_DEFINE_ALTERNATIVE(" ")

#define RUNPATCH_DEFINE_DEFAULT(code) \
    "runpatch_emit_default %=, " #code " \n\t"

// Because the code strings cannot be empty, define this convenience macro.
#define RUNPATCH_DEFINE_DEFAULT_NOP() \
    RUNPATCH_DEFINE_DEFAULT(" ")

#define RUNPATCH_FINISH() \
    "runpatch_finish %= \n\t"


// Convenience macro for simple two-alternative cases.
#define RUNPATCH_ALTERNATIVES(key, enabled, default) \
    RUNPATCH_START(key) \
    RUNPATCH_DEFINE_ALTERNATIVE(enabled) \
    RUNPATCH_DEFINE_DEFAULT(default) \
    RUNPATCH_FINISH()

// Convenience macro for simple nop-or-alternative cases.
#define RUNPATCH_IF_ENABLED(key, enabled) \
    RUNPATCH_START(key) \
    RUNPATCH_DEFINE_ALTERNATIVE(enabled) \
    RUNPATCH_DEFINE_DEFAULT_NOP() \
    RUNPATCH_FINISH()

// Convenience macro for simple default-or-nop cases.
#define RUNPATCH_IF_NOT_ENABLED(key, default) \
    RUNPATCH_START(key) \
    RUNPATCH_DEFINE_ALTERNATIVE_NOP() \
    RUNPATCH_DEFINE_DEFAULT(default) \
    RUNPATCH_FINISH()


#ifdef __cplusplus


#include <cstdint>


namespace System::Kernel::Runpatch
{


struct alternative_info
{
    std::uintptr_t  location;
    std::size_t     size;
};


struct control
{
    const char*             key;
    std::uintptr_t          ip;
    std::size_t             length;
    const alternative_info* alternatives;
    std::size_t             alternative_count;
    alternative_info        original;
    std::uintptr_t          flags;
};


} // namespace System::Kernel::Runpatch


#endif /* ifdef __cplusplus */


#endif /* ifndef __SYSTEM_KERNEL_RUNPATCH_ARCH_X86_RUNPATCH_H */

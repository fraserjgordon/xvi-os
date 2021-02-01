#pragma once
#ifndef __SYSTEM_ABI_DWARF_EXPRESSION_H
#define __SYSTEM_ABI_DWARF_EXPRESSION_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Function.hh>

#if !__SYSTEM_ABI_DWARF_MINIMAL
#  include <System/ABI/Dwarf/FDE.hh>
#  include <System/C++/Utility/String.hh>
#endif

#include <System/ABI/Dwarf/Private/Config.hh>


namespace System::ABI::Dwarf
{


enum class dwarf_op : std::uint8_t
{
    addr        = 0x03, // (location description) Constant address (uintptr_t-size)
    deref       = 0x06, // Dereferences top as an address and replaces it with the uintptr_t from that address
    const1u     = 0x08, // 1 byte unsigned constant
    const1s     = 0x09, // 1 byte signed constant
    const2u     = 0x0a, // 2 byte unsigned constant
    const2s     = 0x0b, // 2 byte signed constant
    const4u     = 0x0c, // 4 byte unsigned constant
    const4s     = 0x0d, // 4 byte signed constant
    const8u     = 0x0e, // 8 byte unsigned constant
    const8s     = 0x0f, // 8 byte singed constant
    constu      = 0x10, // ULEB128 constant
    consts      = 0x11, // SLEB128 constant
    dup         = 0x12, // Duplicates top entry on expression stack
    drop        = 0x13, // Drops top entry from expression stack
    over        = 0x14, // Equivalent to pick with n=1
    pick        = 0x15, // Duplicates the n'th-from-the-top stack element
    swap        = 0x16, // Swaps the top two stack entries
    rot         = 0x17, // Rotate the top three stack entries upwards.
    xderef      = 0x18, // Like deref but also pops the second entry as an address space identifier
    abs         = 0x19, // Calculates the absolute value of the top entry
    _and        = 0x1a, // Bitwise and of the top two entries
    div         = 0x1b, // Divides the second entry by the top entry
    minus       = 0x1c, // Subtracts the top entry from the second entry
    mod         = 0x1d, // Calculates the second entry modulo the top entry
    mul         = 0x1e, // Multiplies the top two entries
    neg         = 0x1f, // Negates the top entry
    _not        = 0x20, // Bitwise not of the top entry
    _or         = 0x21, // Bitwise or of the top two entries
    plus        = 0x22, // Adds the top two entries
    plus_uconst = 0x23, // Adds a ULEB128 operand to the top entry
    shl         = 0x24, // Shifts the second entry left by a number of bits given by the top entry
    shr         = 0x25, // Shifts the second entry right by a number of bits given by the top entry
    shra        = 0x26, // Shifts the second entry right with sign extension by a number of bits given by the top entry
    _xor        = 0x27, // Bitwise xor of the top two entries
    skip        = 0x2f, // Unconditional branch given by a two-byte operand
    bra         = 0x28, // Conditional branch given by a two-byte operand if the top entry is non-zero
    eq          = 0x29, // Equality comparison
    ge          = 0x2a, // Greater-or-equal comparison
    gt          = 0x2b, // Greater-than comparison
    le          = 0x2c, // Less-or-equal comparison
    lt          = 0x2d, // Less-than comparison
    ne          = 0x2e, // Not-equal comparison
    lit0        = 0x30, // Literal 0
    lit1        = 0x31, // Literal 1
    lit2        = 0x32, // Literal 2
    lit3        = 0x33, // Literal 3
    lit4        = 0x34, // Literal 4
    lit5        = 0x35, // Literal 5
    lit6        = 0x36, // Literal 6
    lit7        = 0x37, // Literal 7
    lit8        = 0x38, // Literal 8
    lit9        = 0x39, // Literal 9
    lit10       = 0x3a, // Literal 10
    lit11       = 0x3b, // Literal 11
    lit12       = 0x3c, // Literal 12
    lit13       = 0x3d, // Literal 13
    lit14       = 0x3e, // Literal 14
    lit15       = 0x3f, // Literal 15
    lit16       = 0x40, // Literal 16
    lit17       = 0x41, // Literal 17
    lit18       = 0x42, // Literal 18
    lit19       = 0x43, // Literal 19
    lit20       = 0x44, // Literal 20
    lit21       = 0x45, // Literal 21
    lit22       = 0x46, // Literal 22
    lit23       = 0x47, // Literal 23
    lit24       = 0x48, // Literal 24
    lit25       = 0x49, // Literal 25
    lit26       = 0x4a, // Literal 26
    lit27       = 0x4b, // Literal 27
    lit28       = 0x4c, // Literal 28
    lit29       = 0x4d, // Literal 29
    lit30       = 0x4e, // Literal 30
    lit31       = 0x4f, // Literal 31
    reg0        = 0x50, // (location description) Register 0
    reg1        = 0x51, // (location description) Register 1
    reg2        = 0x52, // (location description) Register 2
    reg3        = 0x53, // (location description) Register 3
    reg4        = 0x54, // (location description) Register 4
    reg5        = 0x55, // (location description) Register 5
    reg6        = 0x56, // (location description) Register 6
    reg7        = 0x57, // (location description) Register 7
    reg8        = 0x58, // (location description) Register 8
    reg9        = 0x59, // (location description) Register 9
    reg10       = 0x5a, // (location description) Register 10
    reg11       = 0x5b, // (location description) Register 11
    reg12       = 0x5c, // (location description) Register 12
    reg13       = 0x5d, // (location description) Register 13
    reg14       = 0x5e, // (location description) Register 14
    reg15       = 0x5f, // (location description) Register 15
    reg16       = 0x60, // (location description) Register 16
    reg17       = 0x61, // (location description) Register 17
    reg18       = 0x62, // (location description) Register 18
    reg19       = 0x63, // (location description) Register 19
    reg20       = 0x64, // (location description) Register 20
    reg21       = 0x65, // (location description) Register 21
    reg22       = 0x66, // (location description) Register 22
    reg23       = 0x67, // (location description) Register 23
    reg24       = 0x68, // (location description) Register 24
    reg25       = 0x69, // (location description) Register 25
    reg26       = 0x6a, // (location description) Register 26
    reg27       = 0x6b, // (location description) Register 27
    reg28       = 0x6c, // (location description) Register 28
    reg29       = 0x6d, // (location description) Register 29
    reg30       = 0x6e, // (location description) Register 30
    reg31       = 0x6f, // (location description) Register 31
    breg0       = 0x70,
    breg1       = 0x71,
    breg2       = 0x72,
    breg3       = 0x73,
    breg4       = 0x74,
    breg5       = 0x75,
    breg6       = 0x76,
    breg7       = 0x77,
    breg8       = 0x78,
    breg9       = 0x79,
    breg10      = 0x7a,
    breg11      = 0x7b,
    breg12      = 0x7c,
    breg13      = 0x7d,
    breg14      = 0x7e,
    breg15      = 0x7f,
    breg16      = 0x80,
    breg17      = 0x81,
    breg18      = 0x82,
    breg19      = 0x83,
    breg20      = 0x84,
    breg21      = 0x85,
    breg22      = 0x86,
    breg23      = 0x87,
    breg24      = 0x88,
    breg25      = 0x89,
    breg26      = 0x8a,
    breg27      = 0x8b,
    breg28      = 0x8c,
    breg29      = 0x8d,
    breg30      = 0x8e,
    breg31      = 0x8f,
    regx        = 0x90, // (location description) Register given by a ULEB128-encoded operand
    fbreg       = 0x91, // Calculates (frame base + SLEB128 offset)
    bregx       = 0x92, // Calculates (ULEB128 register + SLEB128 offset)
    piece       = 0x93, // (location description) Describes the location of part of a larger object
    deref_size  = 0x94, // Like deref but with a 1-byte size operand for the size of the memory access
    xderef_size = 0x95, // Like xderef but with a 1-byte size operand for the size of the memory access
    nop         = 0x96, // Does nothing but take up space
    push_object_address = 0x97, // Pushes the address of the object this expression is being calculated for
    call2       = 0x98, // Calls another DWARF expression given by a two-byte operand
    call4       = 0x99, // Calls another DWARF expression given by a four-byte operand
    call_ref    = 0x9a, // Calls another DWARF expression, potentially in another file
    form_tls_address    = 0x9b, // Converts a thread-local address to an absolute address.
    call_frame_cfa      = 0x9c, // Pushes the current call frame address
    bit_piece   = 0x9d, // (location description) Like piece but with bit (rather than byte) granularity
    implicit_value      = 0x9e, // (location description) Provides an object inline within the expression bytecode
    stack_value = 0x9f, // (location description) Provides an object as the value at the top of the stack

    // GNU extensions.
    gnu_push_tls_address    = 0xe0,
    gnu_uninit              = 0xf0,
    gnu_encoded_addr        = 0xf1,
    gnu_implicit_pointee    = 0xf2,
    gnu_entry_value         = 0xf3,
    gnu_const_type          = 0xf4,
    gnu_regval_type         = 0xf5,
    gnu_deref_type          = 0xf6,
    gnu_convert             = 0xf7,
    gnu_reinterpret         = 0xf9,
    gnu_parameter_ref       = 0xfa,
    gnu_addr_index          = 0xfb,
    gnu_const_index         = 0xfc,
    gnu_variable_value      = 0xfd,
};


// Structure representing a decoded DWARF expression opcode.
struct dwarf_expr_op
{
    // The base opcode. For opcodes that contain their parameter within the opcode byte, this is the "base" version (e.g
    // all of the "litX" opcodes are stored as "lit0" here).
    dwarf_op opcode = dwarf_op::nop;

    // Opcodes can have up to two parameters encoded. They are stored here as uintptr_t but will be bitcast back to the
    // appropriate types when the opcode is executed.
    std::uintptr_t op1 = 0;
    std::uintptr_t op2 = 0;

    // Pointer to and length of the encoded instruction.
    const std::byte* raw = nullptr;
    std::size_t raw_length = 0; 

#if !__SYSTEM_ABI_DWARF_MINIMAL
    std::string toString() const;
#endif
};


//! @brief      Decodes the given stream of DWARF expression instructions.
//! @returns    false on decoding errors; true otherwise.
//!
//! @warning    As expression blocks are loaded as part of an executable's image, they are assumed to be trusted. This
//!             function does check for some decoding errors but it is not comprehensive and is therefore not safe to
//!             run on untrusted data.
__SYSTEM_ABI_DWARF_EXPORT
bool
DwarfExpressionDecode(const std::byte* opcodes, std::size_t len, bool (*callback)(void*, const dwarf_expr_op&), void* callback_context)
__SYSTEM_ABI_DWARF_SYMBOL(DwarfExpressionDecode);

inline bool
DwarfExpressionDecode(const std::byte* opcodes, std::size_t len, std::function<bool(const dwarf_expr_op&)> callback)
{
    auto context_ptr = &callback;
    return DwarfExpressionDecode(opcodes, len, [](void* ctxt, const dwarf_expr_op& arg)
    {
        auto cb = *reinterpret_cast<decltype(context_ptr)>(ctxt);
        return cb(arg);
    }, context_ptr);
}



} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_EXPRESSION_H */

#pragma once
#ifndef __SYSTEM_ABI_DWARF_CFI_H
#define __SYSTEM_ABI_DWARF_CFI_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Function.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Span.hh>

#if !__SYSTEM_ABI_DWARF_MINIMAL
#  include <System/C++/Utility/String.hh>
#endif

#include <System/ABI/Dwarf/Private/Config.hh>
#include <System/ABI/Dwarf/Arch.hh>
#include <System/ABI/Dwarf/FDE.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
class DwarfCIE;


// Masks for extracting the major and minor portions of CFI instruction opcodes.
static constexpr std::uint8_t kDwarfCFIMajorMask    = 0xC0;
static constexpr std::uint8_t kDwarfCFIMinorMask    = 0x3f;


enum class dwarf_cfi_major : std::uint8_t
{
    advance_loc     = 0x40,     // Advances the PC by the encoded value * the code alignment factor.
    offset          = 0x80,     // Register value can be read from a pointer given by the CFA plus a factored offset.
    restore         = 0xC0,     // Changes the rule back to the default from the CIE.
    misc            = 0x00,     // Other operations.
};

enum class dwarf_cfi_misc : std::uint8_t
{
    nop                 = 0x00, // Does nothing.
    set_loc             = 0x01, // Sets the PC to the given address.
    advance_loc1        = 0x02, // Advances the PC by a 1-byte constant * the code alignment factor.
    advance_loc2        = 0x03, // Advances the PC by a 2-byte constant * the code alignment factor.
    advance_loc4        = 0x04, // Advances the PC by a 4-byte constant * the code alignment factor.
    offset_extended     = 0x05, // Like offset major opcode but with separate register parameter.
    restore_extended    = 0x06, // Like restore major opcode but with separate register parameter.
    undefined           = 0x07, // The register cannot be restored.
    same_value          = 0x08, // The register's value is unchanged.
    reg                 = 0x09, // The register's value is stored in another register.
    remember_state      = 0x0a, // Push the current rule set onto a stack.
    restore_state       = 0x0b, // Pop the topmost entry from the rule stack.
    def_cfa             = 0x0c, // Defines the CFA to be the value of the given register and offset.
    def_cfa_register    = 0x0d, // Changes the register (but not offset) used to calculate the CFA.
    def_cfa_offset      = 0x0e, // Changes the offset (but not register) used to calculate the CFA.
    def_cfa_expression  = 0x0f, // The CFA is calculated from a DWARF expression.
    expression          = 0x10, // The register's value is read through a pointer given by a DWARF expression.
    offset_extended_sf  = 0x11, // Like offset_extended but the offset is signed and factored.
    def_cfa_sf          = 0x12, // Like def_cfa but the offset is signed and factored.
    def_cfa_offset_sf   = 0x13, // Like def_cfa_offset but the offset is signed and factored.
    val_offset          = 0x14, // Register value is the value of the CFA plus an offset.
    val_offset_sf       = 0x15, // Like val_offset but the offset is signed and factored.
    val_expression      = 0x16, // The register's value is given by a DWARF expression.

    // MIPS extensions.
    mips_advance_loc8               = 0x1d, // Advances the PC by an 8-byte constant * the code alignment factor.

    // GNU extensions.
    gnu_window_save                 = 0x2d, // SPARC-specific; encodes the behaviour of the "save" opcode.
    gnu_args_size                   = 0x2e, // Specifies the size of the arguments pushed to the stack.
    gnu_negative_offset_extended    = 0x2f, // [OBSOLETE] Same as offset_extended_sf except offset is substracted.

    // These are the major opcodes. They are defined here too to simplify some switch statements.
    advance_loc         = 0x40,
    offset              = 0x80,
    restore             = 0xc0,
};


// Structure representing a decoded DWARF CFI opcode.
struct dwarf_cfi_op
{
    // The base opcode. For major opcodes other than dwarf_cfi_major::misc, this is with the embedded parameter masked
    // out (e.g. all of the advance_loc opcodes are represented here as 0x40).
    std::uint8_t opcode = std::uint8_t(dwarf_cfi_misc::nop);

    // Opcodes can have up to three parameters encoded. They are stored here as uintptr_t but will be bitcast back to the
    // appropriate types when the opcode is executed.
    std::uintptr_t op1 = 0;
    std::uintptr_t op2 = 0;
    std::uintptr_t op3 = 0;

    // Pointer to and length of the encoded instruction.
    const std::byte* raw = nullptr;
    std::size_t raw_length = 0;
};


//! @brief      Decodes a single DWARF CFI instruction.
//! @returns    the decoded instruction or nullopt on error.
//!
//! @warning    As CFI instructions are loaded as part of an executable's image, they are assumed to be trusted. This
//!             function does check for some decoding errors but it is not comprehensive and is therefore not safe to
//!             run on untrusted data.
__SYSTEM_ABI_DWARF_EXPORT
std::optional<dwarf_cfi_op>
CFIDecodeOne(const DwarfCIE& cie, const std::byte* opcodes, std::size_t len)
__SYSTEM_ABI_DWARF_SYMBOL(CFIDecodeOne);


//! @brief      Decodes the given stream of DWARF CFI instructions.
//! @returns    false on decoding errors; true otherwise.
//!
//! @warning    As CFI instructions are loaded as part of an executable's image, they are assumed to be trusted. This
//!             function does check for some decoding errors but it is not comprehensive and is therefore not safe to
//!             run on untrusted data.
__SYSTEM_ABI_DWARF_EXPORT
bool
CFIDecode(const DwarfCIE& cie, const std::byte* opcodes, std::size_t len, bool (*callback)(void*, const dwarf_cfi_op&), void* callback_context)
__SYSTEM_ABI_DWARF_SYMBOL(CFIDecode);

inline bool
CFIDecode(const DwarfCIE& cie, const std::byte* opcodes, std::size_t len, std::function<bool(const dwarf_cfi_op&)> callback)
{
    auto context_ptr = &callback;
    return CFIDecode(cie, opcodes, len, [](void* ctxt, const dwarf_cfi_op& arg)
    {
        auto& cb = *reinterpret_cast<decltype(context_ptr)>(ctxt);
        return cb(arg);
    }, context_ptr);
}


// Forward declaration.
class cfi_iterator;


//! @brief      A class for reading CFI opcodes.
class cfi_decoder
{
public:

    using iterator          = cfi_iterator;
    using const_iterator    = cfi_iterator;

    //! Creates a null decoder.
    constexpr cfi_decoder() = default;

    constexpr cfi_decoder(const cfi_decoder&) = default;
    constexpr cfi_decoder(cfi_decoder&&) = default;
    constexpr cfi_decoder& operator=(const cfi_decoder&) = default;
    constexpr cfi_decoder& operator=(cfi_decoder&&) = default;
    ~cfi_decoder() = default;

    constexpr cfi_decoder(const DwarfCIE& cie, std::span<const std::byte> ops)
        : m_cie(cie),
          m_ops(ops)
    {
    }

    const std::byte* data() const noexcept
    {
        return m_ops.data();
    }

    std::size_t size() const noexcept
    {
        return m_ops.size();
    }

    const DwarfCIE& cie() const noexcept
    {
        return m_cie;
    }

    inline cfi_iterator begin() const;
    inline cfi_iterator end() const;
    inline cfi_iterator cbegin() const;
    inline cfi_iterator cend() const;

private:

    // A CIE struture providing context for decoding.
    DwarfCIE m_cie = {};

    // The range of bytes to be interpreted as CFI opcodes.
    std::span<const std::byte> m_ops = {};
};


//! @brief      A class used for iterating over CFI opcodes.
class cfi_iterator
{
public:

    using iterator_category     = std::forward_iterator_tag;
    using value_type            = dwarf_cfi_op;
    using difference_type       = std::ptrdiff_t;
    using pointer               = const dwarf_cfi_op*;
    using reference             = const dwarf_cfi_op&;

    constexpr cfi_iterator() = default;
    constexpr cfi_iterator(const cfi_iterator&) = default;
    constexpr cfi_iterator(cfi_iterator&&) = default;
    constexpr cfi_iterator& operator=(const cfi_iterator&) = default;
    constexpr cfi_iterator& operator=(cfi_iterator&&) = default;
    ~cfi_iterator() = default;

    cfi_iterator(const cfi_decoder& d, std::size_t offset = 0)
        : m_decoder(std::addressof(d)),
          m_op(CFIDecodeOne(m_decoder->cie(), m_decoder->data() + offset, m_decoder->size() - offset))
    {
    }

    reference operator*() const
    {
        return *m_op;
    }

    pointer operator->() const
    {
        return std::addressof(*m_op);
    }

    cfi_iterator& operator++()
    {
        auto ptr = m_op->raw + m_op->raw_length;
        auto len = (m_decoder->data() + m_decoder->size()) - ptr;
        m_op = CFIDecodeOne(m_decoder->cie(), ptr, len);

        return *this;
    }

    cfi_iterator operator++(int)
    {
        auto tmp = *this;
        operator++();
        return tmp;
    }

    friend constexpr bool operator==(const cfi_iterator& a, const cfi_iterator& b) noexcept
    {
        return a.m_decoder == b.m_decoder
               && ((!a.m_op.has_value() && !b.m_op.has_value())
                   || (a.m_op->raw == b.m_op->raw));
    }

    friend constexpr bool operator!=(const cfi_iterator& a, const cfi_iterator& b) noexcept
    {
        return !(a == b);
    }

private:

    // The parent decoder.
    const class cfi_decoder* m_decoder = nullptr;

    // The most recently decoded opcode.
    std::optional<dwarf_cfi_op> m_op = std::nullopt;
};


cfi_iterator cfi_decoder::begin() const
{
    return cfi_iterator(*this, 0);
}

cfi_iterator cfi_decoder::end() const
{
    return cfi_iterator(*this, m_ops.size());
}

cfi_iterator cfi_decoder::cbegin() const
{
    return begin();
}

cfi_iterator cfi_decoder::cend() const
{
    return end();
}


// Entry in the frame table describing how to restore the value of a particular register.
struct reg_rule
{
    // Types of restorations.
    enum class type
    {
        undefined,          // Contents not defined; not possible to restore previous value.
        unchanged,          // Contents have not changed; no restoration required.
        cfa_relative,       // Contents are stored at a given CFA-relative offset.
        cfa_relative_value, // Contents are equal to the CFA plus an offset.
        other_reg,          // Another register holds the value.
        expression,         // The address of the value is calculated from a DWARF expression.
        expression_value,   // Like expression but the expression gives the value itself.
    } rule_type = type::undefined;

    union
    {
        // Parameters for CFA-relative restoration.
        struct
        {
            std::ptrdiff_t offset;
        } cfa_relative;

        // Parameters for other-register restoration.
        struct
        {
            std::uintptr_t reg;
            std::ptrdiff_t addend;
        } other_reg;

        // Parameters for restoration via a DWARF expression.
        struct
        {
            const std::byte* expr;  // Expression bytecode.
            std::size_t len;        // Length of the expression bytecode.
        } expression = {nullptr, 0};
    } params = {};

    #if !__SYSTEM_ABI_DWARF_MINIMAL
    std::string toString() const;
    #endif

    // Creates an "undefined" rule.
    static constexpr reg_rule Undefined()
    {
        return reg_rule {.rule_type = type::undefined};
    }

    // Creates an "unchanged" rule.
    static constexpr reg_rule Unchanged()
    {
        return reg_rule {.rule_type = type::unchanged};
    }

    // Creates a "CFA-relative" rule.
    static constexpr reg_rule CfaRelative(std::ptrdiff_t offset)
    {
        return reg_rule {.rule_type = type::cfa_relative, .params{.cfa_relative{.offset = offset}}};
    }

    // Creates a "CFA-relative value" rule.
    static constexpr reg_rule CfaRelativeValue(std::ptrdiff_t offset)
    {
        return reg_rule {.rule_type = type::cfa_relative_value, .params{.cfa_relative{.offset = offset}}};
    }

    // Creates a "same as other register" rule.
    static constexpr reg_rule OtherRegister(std::uintptr_t which, std::ptrdiff_t addend = 0)
    {
        return reg_rule {.rule_type = type::other_reg, .params{.other_reg = {which, addend}}};
    }

    // Creates an "expression" rule.
    static constexpr reg_rule Expression(const std::byte* expr, std::size_t expr_len)
    {
        return reg_rule {.rule_type = type::expression, .params{.expression = {expr, expr_len}}};
    }

    // Creates an "expression value" rule.
    static constexpr reg_rule ExpressionValue(const std::byte* expr, std::size_t expr_len)
    {
        return reg_rule {.rule_type = type::expression_value, .params{.expression = {expr, expr_len}}};
    }
};


template <std::size_t Width>
struct reg_rule_row
{
    static constexpr auto RegisterCount = Width;

    std::size_t arguments_size = 0;
    reg_rule cfa;
    reg_rule registers[Width];
    reg_rule return_address;
};

using reg_rule_row_native = reg_rule_row<FrameTraitsNative::kUnwindRegisterCount>;


__SYSTEM_ABI_DWARF_EXPORT
bool ParseCFIRules(const DwarfFDE&, std::uintptr_t pc, std::size_t* arguments_size, reg_rule* output, std::size_t output_count) __SYSTEM_ABI_DWARF_SYMBOL(ParseCFIRules);

template <std::size_t Width>
bool ParseCFIRules(const DwarfFDE& fde, std::uintptr_t pc, reg_rule_row<Width>& output)
{
    return ParseCFIRules(fde, pc, &output.arguments_size, &output.cfa, Width + 2);
}


#if !__SYSTEM_ABI_DWARF_MINIMAL
//! @brief  Formats a sequence of DWARF CFI opcodes in a human-readable manner (akin to disassembly).
std::string DisassembleCFI(const DwarfCIE& cie, const std::byte* cfi, std::size_t length);

std::string PrintRegisterRulesTable(const DwarfFDE& fde);
#endif


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_CFI_H */

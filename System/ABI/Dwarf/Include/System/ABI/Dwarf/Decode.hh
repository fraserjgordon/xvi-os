#pragma once
#ifndef __SYSTEM_ABI_DWARF_DECODE_H
#define __SYSTEM_ABI_DWARF_DECODE_H


#include <System/C++/Utility/Algorithm.hh>

#include <System/ABI/Dwarf/LEB128.hh>


namespace System::ABI::Dwarf
{


// Special values for DWARF CFE pointer encodings.
constexpr std::uint8_t kDwarfPtrAbs  = 0x00;            // Read absolute pointer directly.
constexpr std::uint8_t kDwarfPtrOmit = 0xFF;            // No pointer encoded.

// Bitmasks for extracting various pieces of information from DWARF CFE pointer encodings.
constexpr std::uint8_t kDwarfPtrTypeMask    = 0x0F;     // Type used for the pointer storage.
constexpr std::uint8_t kDwarfPtrRelMask     = 0x70;     // Specifies what pointer is relative to.
constexpr std::uint8_t kDwarfPtrIndirect    = 0x80;     // Need to dereference pointer to get real value.

// DWARF CFE pointer encoding types.
enum class dw_ptr_type : std::uint8_t
{
    uleb128     = 0x01,
    udata2      = 0x02,
    udata4      = 0x03,
    udata8      = 0x04,
    sleb128     = 0x09,
    sdata2      = 0x0A,
    sdata4      = 0x0B,
    sdata8      = 0x0C,
};

// DWARF CFE pointer base addresses.
enum class dw_ptr_rel : std::uint8_t
{
    abs         = 0x00,     // Not relative to anything.
    pc          = 0x10,     // Relative to program counter.
    text        = 0x20,     // Relative to text section.
    data        = 0x30,     // Relative to data section.
    func        = 0x40,     // Relative to function start.
};


// Utility method for performing unaligned reads.
template <class T>
T UnalignedRead(const std::byte* ptr)
{
    union { T t; std::byte b[sizeof(T)]; } un;
    std::copy_n(ptr, sizeof(T), un.b);
    return un.t;
}


// Utility class for decoding encoded pointers.
template <class UintPtrT>
class DwarfPointerDecoder
{
public:

    // Structure describing the points that the pointer may be relative to.
    struct relative_points
    {
        // There is no entry for the "abs" relative type as it is always relative to zero.
        UintPtrT pc = 0;    // If omitted: calculated automatically from decode pointer.
        UintPtrT text = 0;  // If omitted: text-relative pointers returned as zero.
        UintPtrT data = 0;  // If omitted: data-relative pointers returned as zero.
        UintPtrT func = 0;  // If omitted: function-relative pointers returned as zero.
    };

    // Default constructor equivalent to specifying all relative points as zeroes.
    constexpr DwarfPointerDecoder()
      : DwarfPointerDecoder({.pc = 0, .text = 0, .data = 0, .func = 0})
    {
    }

    // Constructor specifying the relative points.
    constexpr DwarfPointerDecoder(const relative_points& rel)
      : m_relative(rel)
    {
    }

    // Default copy and move semantics.
    DwarfPointerDecoder(const DwarfPointerDecoder&) = default;
    DwarfPointerDecoder(DwarfPointerDecoder&&) = default;
    DwarfPointerDecoder& operator=(const DwarfPointerDecoder&) = default;
    DwarfPointerDecoder& operator=(DwarfPointerDecoder&&) = default;

    // No special behaviour on destruction.
    ~DwarfPointerDecoder() = default;

    // Decodes a pointer with the specified encoding at the given location, updating the location pointer. If any errors
    // occur, the decoded pointer is returned with the value zero (which is unlikely to match any valid pointer);
    // whether the location has been updated in this case is undefined.
    //
    // If the encoding type is kDwarfPtrOmit, zero is returned and location is not updated.
    UintPtrT decode(std::uint8_t encoding, const std::byte*& loc) const
    {
        // Do nothing if the pointer is omitted.
        if (encoding == kDwarfPtrOmit)
            return 0;

        // Read the stored pointer directly if specified.
        if (encoding == kDwarfPtrAbs)
        {
            //! @TODO: alignment issues.
            UintPtrT ptr = *reinterpret_cast<const UintPtrT*>(loc);
            loc += sizeof(UintPtrT);
            return ptr;
        }

        // Keep a copy of the original location in case the encoding is PC-relative.
        UintPtrT original_loc = UintPtrT(loc);

        // Extract the encoding type field and read the raw value. Note that in the general case, correct alignment of
        // the value cannot be assumed.
        //
        //! @TODO: fix alignment issues.
        UintPtrT ptr = 0;
        switch (dw_ptr_type(encoding & kDwarfPtrTypeMask))
        {
            case dw_ptr_type::uleb128:
                ptr = DecodeULEB128(loc);
                break;

            case dw_ptr_type::udata2:
                ptr = *reinterpret_cast<const std::uint16_t*>(loc);
                loc += sizeof(std::uint16_t);
                break;

            case dw_ptr_type::udata4:
                ptr = *reinterpret_cast<const std::uint32_t*>(loc);
                loc += sizeof(std::uint32_t);
                break;

            case dw_ptr_type::udata8:
                ptr = *reinterpret_cast<const std::uint64_t*>(loc);
                loc += sizeof(std::uint64_t);
                break;

            case dw_ptr_type::sleb128:
                ptr = DecodeSLEB128(loc);
                break;

            case dw_ptr_type::sdata2:
                ptr = *reinterpret_cast<const std::int16_t*>(loc);
                loc += sizeof(std::int16_t);
                break;

            case dw_ptr_type::sdata4:
                ptr = *reinterpret_cast<const std::int32_t*>(loc);
                loc += sizeof(std::int32_t);
                break;

            case dw_ptr_type::sdata8:
                ptr = *reinterpret_cast<const std::int64_t*>(loc);
                loc += sizeof(std::int64_t);
                break;

            default:
                // Unrecognised encoding type.
                return 0;
        }

        // Extract the location that the pointer is relative to and adjust the decoded pointer to match.
        switch (dw_ptr_rel(encoding & kDwarfPtrRelMask))
        {
            case dw_ptr_rel::abs:
                // Not relative to anything.
                break;

            case dw_ptr_rel::pc:
                // If the stored value is omitted, take the read pointer as the PC.
                ptr += (m_relative.pc != 0) ? m_relative.pc : original_loc;
                break;

            case dw_ptr_rel::text:
                // If we don't have a text section to be relative to, an error has occurred.
                if (m_relative.text == 0)
                    return 0;
                ptr += m_relative.text;
                break;

            case dw_ptr_rel::data:
                // If we don't have a data section to be relative to, an error has occurred.
                if (m_relative.data == 0)
                    return 0;
                ptr += m_relative.data;
                break;

            case dw_ptr_rel::func:
                // If we don't have a function to be relative to, an error has occurred.
                if (m_relative.func == 0)
                    return 0;
                ptr += m_relative.func;
                break;

            default:
                // Unknown relative base.
                return 0;
        }

        // If the pointer is indirect, read the real pointer through the one we've calculated.
        if (encoding & kDwarfPtrIndirect)
            ptr = *reinterpret_cast<const UintPtrT*>(ptr);

        // Return the decoded pointer.
        return ptr;
    }

private:

    // The relative points to use when decoding pointers.
    relative_points m_relative;
};


// Alias of DwarfPointerDecoder for native-sized pointers.
using DwarfPointerDecoderNative = DwarfPointerDecoder<std::uintptr_t>;

// Aliases of DwarfPointerDeocder for 32-bit and 64-bit pointers.
using DwarfPointerDecoder32 = DwarfPointerDecoder<std::uint32_t>;
using DwarfPointerDecoder64 = DwarfPointerDecoder<std::uint64_t>;


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_DECODE_H */

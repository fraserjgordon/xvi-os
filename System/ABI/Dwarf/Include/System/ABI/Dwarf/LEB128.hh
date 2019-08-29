#pragma once
#ifndef __SYSTEM_ABI_DWARF_LEB128_H
#define __SYSTEM_ABI_DWARF_LEB128_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{


// Decodes a ULEB128 byte sequence.
constexpr std::uintptr_t DecodeULEB128(const std::byte*& ptr)
{
    using std::size_t;
    using std::uint8_t;
    using std::uintptr_t;

    uintptr_t value = 0;
    size_t shift = 0;
    while (true)
    {
        auto b = uint8_t(*ptr++);
        value |= ((b & 0x7f) << shift);
        if ((b & 0x80) == 0)
            return value;
        shift += 7;
    }
}

// Decodes a SLEB128 byte sequence.
constexpr std::intptr_t DecodeSLEB128(const std::byte*& ptr)
{
    using std::size_t;
    using std::uint8_t;
    using std::intptr_t;

    intptr_t value = 0;
    size_t shift = 0;
    uint8_t b = 0;
    while (true)
    {
        b = uint8_t(*ptr++);
        value |= ((b & 0x7f) << shift);
        shift += 7;
        if ((b & 0x80) == 0)
            break;
    }

    // Sign-extend the result.
    constexpr size_t digits = std::numeric_limits<intptr_t>::digits;
    if (shift < digits && (b & 0x40))
        value |= -(1 << shift);

    return value;
}


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_LEB128_H */

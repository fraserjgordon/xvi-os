#include <System/ABI/Dwarf/FDE.hh>


#include <System/C++/LanguageSupport/Limits.hh>

#if !__SYSTEM_ABI_DWARF_MINIMAL
#  include <System/C/LanguageSupport/IntTypes.h>
#endif

#include <System/ABI/Dwarf/Arch.hh>
#include <System/ABI/Dwarf/Decode.hh>


using namespace std;
using namespace System::ABI::Dwarf;


bool DwarfCIE::isValid() const
{
    // The CIE is valid if it has been filled in. The code alignment factor is a good proxy for this as it cannot be
    // zero in a valid CIE.
    return (m_codeFactor != 0);
}


DwarfCIE DwarfCIE::DecodeFrom(const std::byte*& ptr, std::uintptr_t text_base, std::uintptr_t data_base)
{
    // Read the length field. If it is all-ones, it is followed by an eight-byte length field containing the real length
    // of the CIE. As such, we must always treat the length as a 64-bit quantity.
    //
    //! @TODO: these reads are not necessarily aligned.
    auto originalPtr = ptr;
    bool is64bit = false;
    uint64_t cieLength = *reinterpret_cast<const uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    if (cieLength == numeric_limits<uint32_t>::max())
    {
        cieLength = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        is64bit = true;
    }

    // If the length is zero, this entry is a terminator instead of a valid CIE.
    if (cieLength == 0)
    {
        // Return an invalid CIE object.
        return {};
    }

    // Now that we've read the length, we're at the start of the CIE (the length does not include the size of the length
    // field itself!).
    auto cieStart = ptr;

    // Read the CIE ID. This is always zero (if not, this is actually an FDE).
    //! @TODO: 64-bit support.
    auto id = *reinterpret_cast<const uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    if (id != 0)
    {
        // This isn't actually a CIE.
        return {};
    }

    // The first field of the CIE is a one-byte version field. These version numbers differ between .eh_frame sections
    // and .debug_frame sections.
    //
    // The defined CIE version numbers are 1, 3 and 4. The only difference between 1 and 3 is the encoding of the return
    // address register field (unsigned byte for version 1, ULEB128 for version 3). Version 4 adds two more fields
    // after the augmentation string.
    uint8_t cieVersion = uint8_t(*ptr++);
    if (cieVersion != 1 && cieVersion != 3 && cieVersion != 4)
    {
        // Unrecognised version; return an invalid CIE.
        return {};
    }

    // Read the augmentation string. It is a normal NUL-terminated string.
    array<char, 5> augString = {};
    size_t augStringLength = 0;
    while (*ptr != byte('\0'))
    {
        // Check that we recognise this character.
        switch (char(*ptr))
        {
            case 'z':
            case 'L':
            case 'P':
            case 'R':
            case 'S':
                break;

            default:
                // Unrecognised augmentation string character; return an invalid CIE.
                return {};
        }

        // Copy the character into our cache.
        augString[augStringLength] = char(*ptr);

        // Move on to the next character.
        ptr++;
        augStringLength++;

        // Have we exceeded the maximum augmentation string length that we're prepared to handle?
        if (augStringLength >= augString.size())
        {
            // There is something wrong with this augmentation string and we can't parse the CIE so bail.
            return {};
        }
    }

    // Skip the NUL at the end of the augmentation string.
    ptr++;

    // If the augmentation string has more than five characters or contains 'L','P','R' without having 'z' as the
    // initial character, it is invalid.
    //
    //! @TODO: check for repeated characters.
    if (augStringLength > 5 || (augStringLength > 1 && augString[0] != 'z'))
    {
        // Return an invalid CIE.
        return {};
    }

    std::uint8_t addressSize = 0;
    std::uint8_t segmentSize = 0;
    if (cieVersion >= 4)
    {
        // Read the pointer size fields.
        addressSize = static_cast<std::uint8_t>(*ptr++);
        segmentSize = static_cast<std::uint8_t>(*ptr++);   
    }

    // Read the code and data alignment factors. It is quite common for the data alignment factor to be negative as it
    // shortens the CFA instruction sequences for machines with downward-growing stacks.
    auto codeFactor = DecodeULEB128(ptr);
    auto dataFactor = DecodeSLEB128(ptr);
    if (codeFactor == 0 || dataFactor == 0)
    {
        // At least one of the factors is invalid; return an invalid CIE.
        return {};
    }

    // Read the column number of the return address register.
    uint32_t returnReg = DecodeULEB128(ptr);

    // Decoder for relative pointers.
    DwarfPointerDecoderNative decoder {{.text = text_base, .data = data_base}};

    // If the first character of the augmentation string was 'z' then we now have the augmentation data.
    uint8_t fdeEncoding = kDwarfPtrAbs;
    uint8_t lsdaEncoding = kDwarfPtrOmit;
    bool signalFrame = false;
    const void* personality = nullptr;
    if (augString[0] == 'z')
    {
        // The augmentation data starts with a ULEB128 quantity storing the augmentation data length.
        auto augDataLength = DecodeULEB128(ptr);

        // Keep a pointer to the start of the augmentation data.
        auto augDataStart = ptr;

        // Process the rest of the augmentation data, using the augmentation string as a key.
        for (size_t i = 1; i < augStringLength; ++i)
        {
            // Which augmentation type are we processing?
            switch (augString[i])
            {
                case 'L':
                {
                    // Read one byte giving the encoding of the LSDA pointer in the FDEs.
                    lsdaEncoding = uint8_t(*ptr++);
                    break;
                }

                case 'P':
                {
                    // Read a byte giving the encoding of the personality routine pointer.
                    auto personalityEncoding = uint8_t(*ptr++);

                    // Read the personality pointer.
                    auto p = decoder.decode(personalityEncoding, ptr);
                    if (!p)
                    {
                        // Failed to decode the personality pointer; return an invalid CIE.
                        return {};
                    }

                    personality = reinterpret_cast<const void*>(p);
                    break;
                }

                case 'R':
                {
                    // Read one byte giving the encoding of pointers in the FDEs.
                    fdeEncoding = uint8_t(*ptr++);
                    break;
                }

                case 'S':
                {
                    // This is a signal frame.
                    signalFrame = true;
                    break;
                }

                default:
                {
                    // This shouldn't have happened, given the augmentation string checks above...
                    return {};
                }
            }
        }

        // Check that we read all of the augmentation data.
        if (augDataStart + augDataLength != ptr)
        {
            // We've failed to correctly parse the augmentation data and can't return a valid CIE.
            return {};
        }
    }

    // Any remaining bytes in the CIE are the initial CFA instructions for associated FDEs. Check that the length of the
    // instruction sequence is non-negative.
    if (cieStart + cieLength < ptr)
    {
        // We've somehow mis-parsed the CIE and cannot trust the values we've extracted.
        return {};
    }

    // Create and return the CIE object.
    DwarfCIE cie;
    cie.m_textBase = text_base;
    cie.m_dataBase = data_base;
    cie.m_location = originalPtr;
    cie.m_length = cieLength;
    cie.m_version = cieVersion;
    cie.m_codeFactor = codeFactor;
    cie.m_dataFactor = dataFactor;
    cie.m_returnRegister = returnReg;
    cie.m_addressSize = addressSize;
    cie.m_segmentSize = segmentSize;
    cie.m_augmentations = augString;
    cie.m_lsdaEncoding = lsdaEncoding;
    cie.m_fdeEncoding = fdeEncoding;
    cie.m_signalFrame = signalFrame;
    cie.m_personalityRoutine = personality;
    cie.m_instructions = ptr;
    cie.m_instructionsLength = (cieStart + cieLength) - ptr;
    ptr = cieStart + cieLength;
    return cie;
};


#if 0 && !__SYSTEM_ABI_DWARF_MINIMAL
string DwarfCIE::toString() const
{
    return Printf
    (
        "CIE@%#" PRIxPTR ":\n"
        "| - length:          %" PRIu64 "\n"
        "| - version:         %" PRIu8 "\n"
        "| - code factor:     %d\n"
        "| - data factor:     %d\n"
        "| - return register: %d (%s)\n"
        "| - augmentations:   %s\n"
        "| - lsda encoding:   %#02hhx (%s)\n"
        "| - fde encoding:    %#02hhx (%s)\n"
        "| - signal frame:    %s\n"
        "| - personality:     %#" PRIxPTR "\n"
        "| - initial insns:   %ju bytes @%#" PRIxPTR "\n",
        uintptr_t(m_location),
        m_length,
        m_version,
        m_codeFactor,
        m_dataFactor,
        m_returnRegister, FrameTraitsNative::GetRegisterName(FrameTraitsNative::reg_enum_t(m_returnRegister)),
        m_augmentations.data(),
        m_lsdaEncoding, PointerEncodingName(m_lsdaEncoding).c_str(),
        m_fdeEncoding, PointerEncodingName(m_fdeEncoding).c_str(),
        (m_signalFrame) ? "yes" : "no",
        uintptr_t(m_personalityRoutine),
        m_instructionsLength, uintptr_t(m_instructions)
    );
}
#endif // if !__SYSTEM_ABI_DWARF_MINIMAL


bool DwarfFDE::isValid() const
{
    // Use the validity of the CIE as a proxy for the FDE's validity.
    return m_cie.isValid();
}


DwarfFDE DwarfFDE::DecodeFrom(const std::byte*& ptr, std::uintptr_t text_base, std::uintptr_t data_base, const void* frame_section_start)
{
    // Read the length field. If it is all-ones, it is followed by an eight-byte length field containing the real length
    // of the FDE. As such, we must always treat the length as a 64-bit quantity.
    //
    //! @TODO: these reads are not necessarily aligned.
    auto originalPtr = ptr;
    uint64_t fdeLength = *reinterpret_cast<const uint32_t*>(ptr);
    bool is64bit = false;
    ptr += sizeof(uint32_t);
    if (fdeLength == numeric_limits<uint32_t>::max())
    {
        fdeLength = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        is64bit = true;
    }

    // We're now at the start of the FDE proper. Keep a pointer to this for reference.
    auto fdeStart = ptr;

    // Read the FDE ID (this field is always 4 bytes). 
    //
    // For .eh_frame sections, this is the number of bytes to go backwards from the start of the FDE to get the parent
    // CIE.
    //
    // For .debug_frame entries, this is the offset of the CIE from the start of the .debug_frame section.
    int64_t cieOffset;
    if (is64bit)
    {
        cieOffset = *reinterpret_cast<const int64_t*>(ptr);
        ptr += sizeof(int64_t);
    }
    else
    {
        cieOffset = *reinterpret_cast<const int32_t*>(ptr);
        ptr += sizeof(int32_t);
    }

    // Read the CIE; we need its contents in order to parse the rest of the FDE.
    const std::byte* ciePtr;
    if (frame_section_start == nullptr)
    {
        ciePtr = fdeStart - cieOffset;
    }
    else
    {
        ciePtr = reinterpret_cast<const byte*>(frame_section_start) + cieOffset;
    }

    auto cie = DwarfCIE::DecodeFrom(ciePtr, text_base, data_base);
    if (!cie)
    {
        // Failed to parse the CIE.
        return {};
    }

    // Next up is the starting address of the range of instructions to which this FDE applies. The encoding method for
    // this address is specified in the CIE.
    DwarfPointerDecoderNative decoder{{.text = text_base, .data = data_base}};
    auto startAddress = decoder.decode(cie.getFDEPointerEncoding(), ptr);
    if (!startAddress)
    {
        // Failed to decode the starting address.
        return {};
    }

    // We now know the function address so decoding function-relative pointers is possible.
    decoder = {{.text = text_base, .data = data_base, .func = startAddress}};

    // And following that is the number of bytes of instructions that are covered by this FDE. Note that we mask the
    // encoding byte first as it doesn't make sense for a count value to be relative to anything (nor indirect).
    auto instructionBytes = decoder.decode(cie.getFDEPointerEncoding() & kDwarfPtrTypeMask, ptr);
    if (instructionBytes == 0)
    {
        // Failed to decode the number of instruction bytes.
        return {};
    }

    // Read the CIE augmentation string as it specifies what (if anything) comes next.
    const byte* lsda = nullptr;
    if (cie.getAugmentations()[0] == 'z')
    {
        // The augmentation data starts with a ULEB128 quantity storing the augmentation data length.
        auto augDataLength = DecodeULEB128(ptr);

        // Keep a pointer to the start of the augmentation data.
        auto augDataStart = ptr;

        // Process the rest of the augmentation data, using the augmentation string as a key.
        const auto& augString = cie.getAugmentations();
        for (size_t i = 1; i < augString.size(); ++i)
        {
            // Which augmentation type are we processing?
            switch (augString[i])
            {
                case 'L':
                {
                    // If, for some reason, the LSDA encoding was explicitly specified as omitted, ignore this.
                    if (cie.getLSDAEncoding() == kDwarfPtrOmit)
                        break;

                    // Read the LSDA address.
                    lsda = reinterpret_cast<const byte*>(decoder.decode(cie.getLSDAEncoding(), ptr));
                    if (lsda == nullptr)
                    {
                        // Failed to decode the LSDA address.
                        return {};
                    }
                    break;
                }

                case 'P':
                case 'R':
                case 'S':
                {
                    // There is nothing stored in the FDE for these augmentations.
                    break;
                }

                case '\0':
                {
                    // Ignore NULs.
                    break;
                }

                default:
                {
                    // This shouldn't have happened, given the augmentation string checks above...
                    return {};
                }
            }
        }

        // Check that we read all of the augmentation data.
        if (augDataStart + augDataLength != ptr)
        {
            // We've failed to correctly parse the augmentation data and can't return a valid CIE.
            return {};
        }
    }

    // Any remaining bytes in the FDE are the CFA instructions.
    //
    // Check that the length of the instruction sequence is non-negative.
    if (fdeStart + fdeLength < ptr)
    {
        // We've somehow mis-parsed the FDE and cannot trust the values we've extracted.
        return {};
    }

    // Create and return the FDE object.
    DwarfFDE fde;
    fde.m_location = originalPtr;
    fde.m_length = fdeLength;
    fde.m_cie = cie;
    fde.m_codeStart = reinterpret_cast<const byte*>(startAddress);
    fde.m_codeLength = instructionBytes;
    fde.m_lsda = lsda;
    fde.m_instructions = ptr;
    fde.m_instructionsLength = static_cast<std::size_t>((fdeStart + fdeLength) - ptr);
    ptr = fdeStart + fdeLength;
    return fde;
}


#if 0 && !__SYSTEM_ABI_DWARF_MINIMAL
string DwarfFDE::toString() const
{
    return Printf
    (
        "FDE@%#" PRIxPTR ":\n"
        "| - length:       %" PRIu64 "\n"
        "| - cie:          %#" PRIxPTR "\n"
        "| - code from:    %#" PRIxPTR "\n"
        "| - code to:      %#" PRIxPTR "\n"
        "| - lsda:         %#" PRIxPTR "\n"
        "| - instructions: %ju bytes @%#" PRIxPTR "\n",
        uintptr_t(m_location),
        m_length,
        uintptr_t(m_cie.getLocation()),
        uintptr_t(m_codeStart),
        uintptr_t(m_codeStart) + m_codeLength,
        uintptr_t(m_lsda),
        m_instructionsLength, uintptr_t(m_instructions)
    );
}
#endif // if !__SYSTEM_ABI_DWARF_MINIMAL


DwarfCieOrFde DwarfCieOrFde::DecodeFrom(const byte*& ptr, std::uintptr_t text_base, std::uintptr_t data_base, const void* frame_section_start)
{
    // Read the length field. If it is all-ones, it is followed by an eight-byte length field containing the real length
    // of the CIE/FDE. As such, we must always treat the length as a 64-bit quantity.
    //
    //! @TODO: these reads are not necessarily aligned.
    auto originalPtr = ptr;
    uint64_t length = *reinterpret_cast<const uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    if (length == numeric_limits<uint32_t>::max())
    {
        length = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
    }

    // If we have a zero length, we're at the end of the CIE/FDE list so return an invalid CIE.
    if (length == 0)
    {
        return {};
    }

    // Read the CIE/FDE ID (this field is always 4 bytes). This is the number of bytes to go backwards from the start of
    // the FDE to get the parent CIE and is zero for CIEs.
    //
    //! @TODO: support for .debug_frame entries; this ID is generally an absolute value there.
    //! @TODO: 64-bit support.
    int32_t cieOffset = *reinterpret_cast<const int32_t*>(ptr);

    // Decode as an FDE or CIE as appropriate.
    ptr = originalPtr;
    if (cieOffset == 0)
    {
        return DwarfCieOrFde{.cie = DwarfCIE::DecodeFrom(ptr, text_base, data_base)};
    }
    else
    {
        return DwarfCieOrFde{.is_fde = true, .fde = DwarfFDE::DecodeFrom(ptr, text_base, data_base, frame_section_start)};
    }
}

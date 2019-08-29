#pragma once
#ifndef __SYSTEM_ABI_DWARF_FDE_H
#define __SYSTEM_ABI_DWARF_FDE_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/String.hh>

#include <System/ABI/Dwarf/Decode.hh>


namespace System::ABI::Dwarf
{


class DwarfCIE
{
public:

    // A default-constructed CIE is used as an invalid CIE.
    constexpr DwarfCIE() = default;

    // All of the pointers point to static data so copying is shallow.
    DwarfCIE(const DwarfCIE&) = default;
    DwarfCIE(DwarfCIE&&) = default;
    DwarfCIE& operator=(const DwarfCIE&) = default;
    DwarfCIE& operator=(DwarfCIE&&) = default;
    ~DwarfCIE() = default;

    // Returns true if this CIE object is valid.
    bool isValid() const;
    explicit operator bool() const
    {
        return isValid();
    }

    const std::byte* getLocation() const
    {
        return m_location;
    }

    std::uint64_t getLength() const
    {
        return m_length;
    }

    std::uint8_t getVersion() const
    {
        return m_version;
    }

    std::uint32_t getCodeFactor() const
    {
        return m_codeFactor;
    }

    std::int32_t getDataFactor() const
    {
        return m_dataFactor;
    }

    const auto& getAugmentations() const
    {
        return m_augmentations;
    }

    std::uint8_t getLSDAEncoding() const
    {
        return m_lsdaEncoding;
    }

    std::uint8_t getFDEPointerEncoding() const
    {
        return m_fdeEncoding;
    }

    bool isSignalFrame() const
    {
        return m_signalFrame;
    }

    const void* getPersonalityRoutine() const
    {
        return m_personalityRoutine;
    }

    const std::byte* getInstructions() const
    {
        return m_instructions;
    }

    std::size_t getInstructionsLength() const
    {
        return m_instructionsLength;
    }

#if !__SYSTEM_ABI_DWARF_MINIMAL
    // Formats the CIE into a human-readable form.
    std::string toString() const;
#endif

    static DwarfCIE DecodeFrom(const std::byte*&);

private:

    // Pointer to and length of the original source for the CIE.
    const std::byte* m_location = nullptr;
    std::uint64_t m_length = 0;

    // Version (format) of the CIE.
    std::uint8_t m_version = 0;

    // Code and data alignment factors to apply to offsets encoded in CFA instructions. These will always be non-zero
    // after reading the CIE.
    std::uint32_t m_codeFactor = 0;
    std::int32_t  m_dataFactor = 0;

    // The column number of the return address register.
    std::uint32_t m_returnRegister = -1;

    // The contents of the augmentation string. Because we only know about 5 possible characters in the augmentation
    // string, a fixed-size buffer is used. These are needed to be able to read the associated FDE.
    //
    // The recognised values are:
    //   'z' - must be first if any other characters are present.
    //   'L' - indicates the presence of a language-specific data area (LSDA) pointer in the FDE.
    //   'P' - indicates the presence of a personality routine in the FDE.
    //   'R' - indicates the presence of an FDE pointer encoding byte in this CIE.
    //   'S' - indicates that this frame is a signal frame.
    std::array<char, 5> m_augmentations = {};

    // The encoding used for the LDSA pointer stored in the associated FDEs. This is omitted by default but may be
    // specified if the augmentation string contains 'L'.
    std::uint8_t m_lsdaEncoding = kDwarfPtrOmit;

    // The encoding used for pointers in FDEs associated with this CIE. This defaults to an absolute pointer but may be
    // changed by a value in the augmentation data (only if the augmentation string contains 'R').
    std::uint8_t m_fdeEncoding = kDwarfPtrAbs;

    // Set to true if this CIE represents a signal frame.
    bool m_signalFrame = false;

    // A pointer to the personality routine associated with this CIE. This is null by default but may be specified if
    // the augmentation string contains 'P'.
    const void* m_personalityRoutine = nullptr;

    // Pointer to and size of the initial CFA instructions to be executed before each FDE's instructions.
    const std::byte* m_instructions = nullptr;
    std::size_t m_instructionsLength = 0;
};


class DwarfFDE
{
public:

    // A default-constructed FDE is used as an invalid fdE.
    constexpr DwarfFDE() = default;

    // All of the pointers point to static data so copying is shallow.
    DwarfFDE(const DwarfFDE&) = default;
    DwarfFDE(DwarfFDE&&) = default;
    DwarfFDE& operator=(const DwarfFDE&) = default;
    DwarfFDE& operator=(DwarfFDE&&) = default;
    ~DwarfFDE() = default;

    // Returns true if this CIE object is valid.
    bool isValid() const;
    explicit operator bool() const
    {
        return isValid();
    }

    const DwarfCIE& getCIE() const
    {
        return m_cie;
    }

    const std::byte* getCodeRangeStart() const
    {
        return m_codeStart;
    }

    std::size_t getCodeRangeSize() const
    {
        return m_codeLength;
    }

    const std::byte* getLSDA() const
    {
        return m_lsda;
    }

    const std::byte* getInstructions() const
    {
        return m_instructions;
    }

    std::size_t getInstructionsLength() const
    {
        return m_instructionsLength;
    }

#if !__SYSTEM_ABI_DWARF_MINIMAL
    // Formats the FDE into a human-readable form.
    std::string toString() const;
#endif

    static DwarfFDE DecodeFrom(const std::byte*&);

private:

    // The Common Information Entry (CIE) associated with this FDE.
    DwarfCIE m_cie = {};

    // Pointer to and length of the original FDE.
    const std::byte* m_location = nullptr;
    std::uint64_t m_length = 0;

    // The range of code to which this FDE applies.
    const std::byte* m_codeStart = nullptr;
    std::size_t m_codeLength = 0;

    // Pointer to the LSDA.
    const std::byte* m_lsda = nullptr;

    // Pointer to and size of the instructions describing how to unwind this call frame. These instructions are executed
    // after the instructions in the CIE have been executed (i.e the CIE instructions act as if they're prefixed to the
    // instructions for each associated FDE).
    const std::byte* m_instructions = nullptr;
    std::size_t m_instructionsLength = 0;
};


// Holds either a CIE or FDE, depending on what is encoded.
struct DwarfCieOrFde
{
    // True if this contains an FDE; otherwise it contains a CIE.
    bool is_fde = false;

    union
    {
        DwarfCIE cie = {};
        DwarfFDE fde;
    };

    // Decodes a CIE or FDE, as appropriate, from the given pointer.
    static DwarfCieOrFde DecodeFrom(const std::byte*& ptr);
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_FDE_H */

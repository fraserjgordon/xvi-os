#ifndef __SYSTEM_EXECUTABLE_IMAGE_ELF_TYPES_H
#define __SYSTEM_EXECUTABLE_IMAGE_ELF_TYPES_H


#include <array>
#include <cstddef>
#include <cstdint>

#include <System/Utility/Endian/Endian.hh>


//! @TODO: add [[nodiscard]] to all byte_swap methods.


namespace System::Executable::Image::ELF
{


using System::Utility::Endian::fixed_endian_type;


template <std::endian Endian>
struct elf32_basic_traits
{
    template <class T>
    using endian            = fixed_endian_type<T, Endian>;   

    using byte_swapped_type = elf32_basic_traits<System::Utility::Endian::opposite_endian(Endian)>;

    using address_t         = endian<std::uint32_t>;
    using offset_t          = endian<std::uint32_t>;
    using half_t            = endian<std::uint16_t>;
    using word_t            = endian<std::uint32_t>;
    using sword_t           = endian<std::int32_t>;
};

using elf32le_basic_traits = elf32_basic_traits<std::endian::little>;
using elf32be_basic_traits = elf32_basic_traits<std::endian::big>;
using elf32ne_basic_traits = elf32_basic_traits<std::endian::native>;

template <std::endian Endian>
struct elf64_basic_traits
{
    template <class T>
    using endian            = fixed_endian_type<T, Endian>;

    using byte_swapped_type = elf64_basic_traits<System::Utility::Endian::opposite_endian(Endian)>;

    using address_t         = endian<std::uint64_t>;
    using offset_t          = endian<std::uint64_t>;
    using half_t            = endian<std::uint16_t>;
    using word_t            = endian<std::uint32_t>;
    using sword_t           = endian<std::int32_t>;
    using xword_t           = endian<std::uint64_t>;
    using sxword_t          = endian<std::int64_t>;
};

using elf64le_basic_traits = elf64_basic_traits<std::endian::little>;
using elf64be_basic_traits = elf64_basic_traits<std::endian::big>;
using elf64ne_basic_traits = elf64_basic_traits<std::endian::native>;


enum class FormatVersion : std::uint8_t
{
    None            = 0,    // Not a valid file format version.
    v1_0            = 1,    // First version of the ELF format.
    Current         = v1_0, // Maximum version currently supported.
};

enum class FileClass : std::uint8_t
{
    None            = 0,    // Not a valid file class.
    Class32         = 1,    // 32-bit file format.
    Class64         = 2,    // 64-bit file format.
};

enum class ByteOrder : std::uint8_t
{
    None            = 0,    // Not a valid byte order.
    LE              = 1,    // Little-endian format.
    BE              = 2,    // Big-endian format.
};

enum class OSABI : std::uint8_t
{
    Generic         = 0,    // No extensions defined (plain SysV).
    GNU             = 3,    // GNU extensions present.
    Standalone      = 255,  // Standalone/embedded (i.e no host OS).
};

enum class ObjectType : std::uint16_t
{
    None            = 0,    // Not a valid object type.
    Relocatable     = 1,    // Not-fully-linked file.
    Executable      = 2,    // Executable image.
    Dynamic         = 3,    // Dynamic library (or PIC executable).
    Core            = 4,    // Core dump.

    LowOS           = 0xFE00,   // Start of OS-specific range.
    HighOS          = 0xFEFF,   // End of OS-specific range.

    LowProc         = 0xFF00,   // Start of processor-specific range.
    HighProc        = 0xFFFF,   // End of processor-specific range.
};

enum class ProcessorType : std::uint16_t
{
    None            = 0,        // Not a valid machine type.

    SPARC           = 2,
    x86             = 3,
    MIPS            = 8,
    PowerPC         = 20,
    PowerPC64       = 21,
    ARM             = 40,
    SPARCv9         = 43,
    x86_64          = 62,
    AArch64         = 183,
};

enum class CompressionType : std::uint32_t
{
    ZLib            = 0,        // ZLib compression algorithm.

    LowOS           = 0x60000000,   // Start of OS-specific range.
    HighOS          = 0x6fffffff,   // End of OS-specific range.

    LowProc         = 0x70000000,   // Start of processor-specific range.
    HighProc        = 0x7fffffff,   // End of processor-specific range.
};

enum class SymbolType : std::uint8_t
{
    None            = 0,        // No type.
    Object          = 1,        // Data object.
    Function        = 2,        // Function.
    Section         = 3,        // Section.
    File            = 4,        // Source file name.
    Common          = 5,        // Uninitialised data.
    TLS             = 6,        // Thread-local data.

    LowOS           = 10,       // Start of OS-specific range.
    HighOS          = 12,       // End of OS-specific range.

    LowProc         = 13,       // Start of processor-specific range.
    HighProc        = 15,       // End of processor-specific range.
};

enum class SymbolBinding : std::uint8_t
{
    Local           = 0,        // Not visible from other object files.
    Global          = 1,        // Visible everywhere.
    Weak            = 2,        // Like global but has lower priority.

    LowOS           = 10,       // Start of OS-specific range.
    HighOS          = 12,       // End of OS-specific range.

    LowProc         = 13,       // Start of processor-specific range.
    HighProc        = 15,       // End of processor-specific range.
};

enum class SymbolVisibility : std::uint8_t
{
    Default         = 0,        // Fully visible and pre-emptable.
    Internal        = 1,        // Defined by processor but stronger than Hidden.
    Hidden          = 2,        // Not visible to other components.
    Protected       = 3,        // Fully visible but not pre-emptable.
};

enum class SectionType : std::uint32_t
{
    Null            = 0,        // Ignored section.
    ProgBits        = 1,        // Contains the program's code or data.
    SymbolTable     = 2,        // Symbol table section.
    StringTable     = 3,        // String table section.
    RelocationsA    = 4,        // Relocations with explicit addends.
    Hash            = 5,        // Symbol hash table.
    Dynamic         = 6,        // Dynamic linking information.
    Note            = 7,        // File marking information.
    NoBits          = 8,        // Like ProgBits but doesn't take space in the image.
    Relocations     = 9,        // Relocations without explicit addends.
    SharedLib       = 10,       // Unspecified semantics.
    DynamicSymbols  = 11,       // Exported (dynamic) symbols.
    InitArray       = 14,       // Pointers to initialisation functions.
    FiniArray       = 15,       // Pointers to finalisation functions.
    PreinitArray    = 16,       // Pointers to early initialisation functions.
    Group           = 17,       // Section group.
    SymbolTableIndex= 18,       // Extended (32-bit) section indices for symbols.

    LowOS           = 0x60000000,   // Start of OS-specific range.
    HighOS          = 0x6fffffff,   // End of OS-specific range.

    LowProc         = 0x70000000,   // Start of processor-specific range.
    HighProc        = 0x7fffffff,   // End of processor-specific range.

    LowUser         = 0x80000000,   // Start of application-defined range.
    HighUser        = 0xffffffff,   // End of application-defined range.

    // GNU-specific section types.
    GNU_VersionsProvided    = 0x6ffffffd,   // Provided symbol versions.
    GNU_VersionsNeeded      = 0x6ffffffe,   // Required symbol versions.
    GNU_SymbolVersions      = 0x6fffffff,   // Symbol version table.
};

enum class SegmentType : std::uint32_t
{
    Null            = 0,            // Unused segment.
    Load            = 1,            // Segment that should be loaded into memory at run-time.
    Dynamic         = 2,            // Contains dynamic linking information.
    Interpreter     = 3,            // Specifies the program's interpreter.
    Note            = 4,            // Auxiliary information.
    SharedLib       = 5,            // Unspecified semantics.
    ProgHeaders     = 6,            // Program header table mapping.
    TLS             = 7,            // Thread-local storage template.

    LowOS           = 0x60000000,   // Start of OS-specific range.
    HighOS          = 0x6fffffff,   // End of OS-specific range.

    LowProc         = 0x70000000,   // Start of processor-specific range.
    HighProc        = 0x7fffffff,   // End of processor-specific range.

    // GNU-specific segment types.
    GNU_EhFrame     = 0x6474e550,   // Exception handling info (.eh_frame_hdr section).
    GNU_StackFlags  = 0x6474e551,   // Executable flag for the program's stack.
    GNU_RelRO       = 0x6474e552,   // Segment that needs relocations but should be read-only at run-time.
};

enum class DynamicTagType : std::uint32_t
{
    Null                = 0,            // Terminates the list of dynamic tags.
    Needed              = 1,            // Identifies a needed shared library.
    PLTRelocationsSize  = 2,            // Total size of all PLT relocation entries.
    PLTGOT              = 3,            // Address of the PLT and/or GOT.
    Hash                = 4,            // Address of the symbol hash table.
    StringTable         = 5,            // Address of the string table.
    SymbolTable         = 6,            // Address of the symbol table.
    RelocationsA        = 7,            // Address of the relocations table (with explicit addends).
    RelocationsASize    = 8,            // Total size of the RelA table.
    RelocationsAEnt     = 9,            // Size of each entry in the RelA table.
    StringSize          = 10,           // Size of the string table.
    SymbolEnt           = 11,           // Size of each entry in the symbol table.
    Init                = 12,           // Address of the initialisation function.
    Fini                = 13,           // Address of the finalisation function.
    Soname              = 14,           // Offset of the SONAME string.
    RPath               = 15,           // Offset of the library search path string (RPATH).
    Symbolic            = 16,           // Alters symbol resolution order.
    Relocations         = 17,           // Address of the relocations table (without explicit addends).
    RelocationsSize     = 18,           // Total size of the Rel table.
    RelocationsEnt      = 19,           // Size of each entry in the Rel table.
    PLTRelocations      = 20,           // Specifies the type of relocations (Rel or RelA) in the PLT.
    Debug               = 21,           // Unspecified debugging information.
    TextRelocations     = 22,           // Indicates relocations to read-only text are present.
    JMPRelocations      = 23,           // Address of the relocations table for PLT entries.
    BindNow             = 24,           // Indicates that symbols should not be resolved lazily.
    InitArray           = 25,           // Address of an array of initialisation function pointers.
    FiniArray           = 26,           // Address of an array of finalisation function pointers.
    InitArraySize       = 27,           // Size of the initialisation array.
    FiniArraySize       = 28,           // Size of the finalisation array.
    RunPath             = 29,           // Offset of the library search path string (RUNPATH).
    Flags               = 30,           // Flags.

    PreinitArray        = 32,           // Address of an array of early initialisation function pointers.
    PreinitArraySize    = 33,           // Size of the early initialisation array.
    SymbolTableIndex    = 34,           // Address of the section with extended symbol section indices.

    LowOS               = 0x60000000,   // Start of OS-specific range.
    HighOS              = 0x6fffffff,   // End of OS-specific range.

    LowProc             = 0x70000000,   // Start of processor-specific range.
    HighProc            = 0x7fffffff,   // End of processor-specific range.

    // GNU-specific dynamic info.
    GNU_PosFlag1            = 0x6ffffdfd,   // ???
    GNU_SymbolInfoSize      = 0x6ffffdfe,   // Symbol info table size.
    GNU_SymbolInfoEnt       = 0x6ffffdff,   // Size of entries in the symbol info table.
    GNU_DepAudit            = 0x6ffffefb,   // Additional shared object to load.
    GNU_Audit               = 0x6ffffefc,   // Additional shared object to load.
    GNU_SymbolInfo          = 0x6ffffeff,   // Symbol info table.
    GNU_SymbolVersions      = 0x6ffffff0,   // Symbol version table.
    GNU_RelocationsACount   = 0x6ffffff9,   // Number of entries in the relocations table (with explicit addends).
    GNU_RelocationsCount    = 0x6ffffffa,   // Number of entries in the relocations table (without explicit addends).
    GNU_Flags1              = 0x6ffffffb,   // ???
    GNU_VersionsProvided    = 0x6ffffffc,   // Provided versions.
    GNU_VersionsProvidedEnt = 0x6ffffffd,   // Number of provided versions.
    GNU_VersionsNeeded      = 0x6ffffffe,   // Imported versions.
    GNU_VersionsNeededEnt   = 0x6fffffff,   // Number of imported versions.

    // Other GNU extensions (originally assiged by Sun to processor-specific values instead of OS-specific).
    GNU_Auxiliary           = 0x7ffffffd,   // Additional shared object to load.
    GNU_Filter              = 0x7fffffff,   // Additional shared object to load.
};

enum class VersionTableRevision : std::uint16_t
{
    None                = 0,            // Invalid version table revision.
    Current             = 1,            // Only defined version table revision.
};


// Special section indices.
constexpr std::uint16_t SectionIndexUndefined   = 0;            // Used to mark undefined symbols.
constexpr std::uint16_t SectionIndexLowReserved = 0xff00;       // Start of reserved indices.
constexpr std::uint16_t SectionIndexLowProc     = 0xff00;       // Start of processor-reserved section indices.
constexpr std::uint16_t SectionIndexHighProc    = 0xff1f;       // End of processor-reserved section indices.
constexpr std::uint16_t SectionIndexLowOS       = 0xff20;       // Start of OS-reserved section indices.
constexpr std::uint16_t SectionIndexHighOS      = 0xff3f;       // End of OS-reserved section indices.
constexpr std::uint16_t SectionIndexAbsolute    = 0xfff1;       // Symbols relative to this section are really absolute addresses.
constexpr std::uint16_t SectionIndexCommon      = 0xfff2;       // Symbols relative to this section are common symbols.
constexpr std::uint16_t SectionIndexXIndex      = 0xffff;       // Escape value used to indicate index > 0xff00.
constexpr std::uint16_t SectionIndexHighReserved= 0xffff;       // End of reserved indices.

// Section attribute flags.
using section_flag_t = std::uint32_t;
constexpr section_flag_t SectionWrite           = 0x00000001;   // Section is writable during image execution.
constexpr section_flag_t SectionAlloc           = 0x00000002;   // Section is present in the image at run-time.
constexpr section_flag_t SectionExec            = 0x00000004;   // Section contains executable code.
constexpr section_flag_t SectionMergeable       = 0x00000010;   // Section contains mergeable entries.
constexpr section_flag_t SectionStrings         = 0x00000020;   // Section contains null-terminated strings.
constexpr section_flag_t SectionInfoLink        = 0x00000040;   // Header "info" member contains a section index.
constexpr section_flag_t SectionLinkOrder       = 0x00000080;   // Header "link" field contains a section index.
constexpr section_flag_t SectionOSNonConforming = 0x00000100;   // Section needs OSABI-specific support to process.
constexpr section_flag_t SectionGroup           = 0x00000200;   // Section is part of a section group.
constexpr section_flag_t SectionTLS             = 0x00000400;   // Section holds thread-local storage (TLS) data.
constexpr section_flag_t SectionCompressed      = 0x00000800;   // Section contains compressed data.
constexpr section_flag_t SectionOSMask          = 0x0ff00000;   // Flags with OSABI-specified meanings.
constexpr section_flag_t SectionProcMask        = 0xf0000000;   // Flags with processor-specified meanings.

// Section group flags.
using section_group_flag_t = std::uint32_t;
constexpr section_group_flag_t SectionGroupComdat   = 0x00000001;   // Section describes a COMDAT group.
constexpr section_group_flag_t SectionGroupOSMask   = 0x0ff00000;   // Flags with OSABI-specified meanings.
constexpr section_group_flag_t SectionGroupProcMask = 0xf0000000;   // Flags with processor-specified meanings.

// Segment flags.
using segment_flag_t = std::uint32_t;
constexpr segment_flag_t SegmentExec        = 0x00000001;       // Segment is executable.
constexpr segment_flag_t SegmentWrite       = 0x00000002;       // Segment is writeable.
constexpr segment_flag_t SegmentRead        = 0x00000004;       // Segment is readable.
constexpr segment_flag_t SegmentOSMask      = 0x0ff00000;       // Flags with OSABI-specified meanings.
constexpr segment_flag_t SegmentProcMask    = 0xf0000000;       // Flags with processor-specified meanings.

// Dynamic flags.
using dynamic_flag_t = std::uint32_t;
constexpr dynamic_flag_t DynamicUsesOrigin      = 0x00000001;       // File uses "$ORIGIN" substitution sequence when calling dlopen.
constexpr dynamic_flag_t DynamicSymbolic        = 0x00000002;       // Alters symbol resolution order to prefer this library over the executable.
constexpr dynamic_flag_t DynamicTextRelocations = 0x00000004;       // Relocations are required to read-only text.
constexpr dynamic_flag_t DynamicBindNow         = 0x00000008;       // Symbol resolution should not happen lazily.
constexpr dynamic_flag_t DynamicStaticTLS       = 0x00000010;       // File cannot be loaded dynamically due to TLS model used.


template <class Traits>
struct basic_section_header_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_section_header_32<typename Traits::byte_swapped_type>;

    Traits::offset_t        name_offset         = 0;
    endian<SectionType>     type                = SectionType::Null;
    Traits::word_t          flags               = 0;
    Traits::address_t       address             = 0;
    Traits::offset_t        offset              = 0;
    Traits::word_t          size                = 0;
    Traits::word_t          link                = 0;
    Traits::word_t          info                = 0;
    Traits::word_t          address_alignment   = 0;
    Traits::word_t          entry_size          = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            name_offset,
            type,
            flags,
            address,
            offset,
            size,
            link,
            info,
            address_alignment,
            entry_size,
        };
    }
};

template <std::endian Endian>
using section_header_32 = basic_section_header_32<elf32_basic_traits<Endian>>;

using section_header_32le = section_header_32<std::endian::little>;
using section_header_32be = section_header_32<std::endian::big>;
using section_header_32ne = section_header_32<std::endian::native>;


template <class Traits>
struct basic_section_header_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_section_header_64<typename Traits::byte_swapped_type>;

    Traits::offset_t        name_offset         = 0;
    endian<SectionType>     type                = SectionType::Null;
    Traits::xword_t         flags               = 0;
    Traits::address_t       address             = 0;
    Traits::offset_t        offset              = 0;
    Traits::word_t          size                = 0;
    Traits::word_t          link                = 0;
    Traits::word_t          info                = 0;
    Traits::xword_t         address_alignment   = 0;
    Traits::xword_t         entry_size          = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            name_offset,
            type,
            flags,
            address,
            offset,
            size,
            link,
            info,
            address_alignment,
            entry_size,
        };
    }
};

template <std::endian Endian>
using section_header_64 = basic_section_header_64<elf64_basic_traits<Endian>>;

using section_header_64le = section_header_64<std::endian::little>;
using section_header_64be = section_header_64<std::endian::big>;
using section_header_64ne = section_header_64<std::endian::native>;


template <class Traits>
struct basic_compression_header_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_compression_header_32<typename Traits::byte_swapped_type>;
    
    endian<CompressionType> type;                   // Compression algorithm.
    Traits::address_t       size            = 0;    // Uncompressed size.
    Traits::word_t          alignment       = 0;    // Required alignment.

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            size,
            alignment,
        };
    }
};

template <std::endian Endian>
using compression_header_32 = basic_compression_header_32<elf32_basic_traits<Endian>>;

using compression_header_32le = compression_header_32<std::endian::little>;
using compression_header_32be = compression_header_32<std::endian::big>;
using compression_header_32ne = compression_header_32<std::endian::native>;


template <class Traits>
struct basic_compression_header_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_compression_header_64<typename Traits::byte_swapped_type>;

    endian<CompressionType> type;                   // Compression algorithm.
    Traits::word_t          reserved        = 0;
    Traits::xword_t         size            = 0;    // Uncompressed size.
    Traits::xword_t         alignment       = 0;    // Required alignment.

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            reserved,
            size,
            alignment,
        };
    }
};

template <std::endian Endian>
using compression_header_64 = basic_compression_header_64<elf64_basic_traits<Endian>>;

using compression_header_64le = compression_header_64<std::endian::little>;
using compression_header_64be = compression_header_64<std::endian::big>;
using compression_header_64ne = compression_header_64<std::endian::native>;


template <class Traits>
struct basic_symbol_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_symbol_32<typename Traits::byte_swapped_type>;    

    Traits::offset_t        name_offset     = 0;
    Traits::word_t          value           = 0;
    Traits::word_t          size            = 0;
    std::uint8_t            info            = 0;
    std::uint8_t            other           = 0;
    Traits::half_t          section         = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            name_offset,
            value,
            size,
            info,
            other,
            section,
        };
    }
};

template <std::endian Endian>
using symbol_32 = basic_symbol_32<elf32_basic_traits<Endian>>;

using symbol_32le = symbol_32<std::endian::little>;
using symbol_32be = symbol_32<std::endian::big>;
using symbol_32ne = symbol_32<std::endian::native>;


template <class Traits>
struct basic_symbol_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_symbol_64<typename Traits::byte_swapped_type>;

    Traits::word_t          name_offset     = 0;
    std::uint8_t            info            = 0;
    std::uint8_t            other           = 0;
    Traits::half_t          section         = 0;
    Traits::xword_t         value           = 0;
    Traits::xword_t         size            = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            name_offset,
            info,
            other,
            section,
            value,
            size,
        };
    }
};

template <std::endian Endian>
using symbol_64 = basic_symbol_64<elf64_basic_traits<Endian>>;

using symbol_64le = symbol_64<std::endian::little>;
using symbol_64be = symbol_64<std::endian::big>;
using symbol_64ne = symbol_64<std::endian::native>;


template <class Traits>
struct basic_relocation_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_relocation_32<typename Traits::byte_swapped_type>;    

    Traits::offset_t        offset          = 0;
    Traits::word_t          info            = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            offset,
            info,
        };
    }
};

template <std::endian Endian>
using relocation_32 = basic_relocation_32<elf32_basic_traits<Endian>>;

using relocation_32le = relocation_32<std::endian::little>;
using relocation_32be = relocation_32<std::endian::big>;
using relocation_32ne = relocation_32<std::endian::native>;


template <class Traits>
struct basic_relocationa_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_relocationa_32<typename Traits::byte_swapped_type>;    

    Traits::offset_t        offset          = 0;
    Traits::word_t          info            = 0;
    Traits::sword_t         addend          = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            offset,
            info,
            addend,
        };
    }
};

template <std::endian Endian>
using relocationa_32 = basic_relocationa_32<elf32_basic_traits<Endian>>;

using relocationa_32le = relocationa_32<std::endian::little>;
using relocationa_32be = relocationa_32<std::endian::big>;
using relocationa_32ne = relocationa_32<std::endian::native>;


template <class Traits>
struct basic_relocation_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_relocation_64<typename Traits::byte_swapped_type>;
    
    Traits::offset_t        offset          = 0;
    Traits::xword_t         info            = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            offset,
            info,
        };
    }
};

template <std::endian Endian>
using relocation_64 = basic_relocation_64<elf64_basic_traits<Endian>>;

using relocation_64le = relocation_64<std::endian::little>;
using relocation_64be = relocation_64<std::endian::big>;
using relocation_64ne = relocation_64<std::endian::native>;


template <class Traits>
struct basic_relocationa_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_relocationa_64<typename Traits::byte_swapped_type>;    

    Traits::offset_t        offset          = 0;
    Traits::xword_t         info            = 0;
    Traits::sxword_t        addend          = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            offset,
            info,
            addend,
        };
    }
};

template <std::endian Endian>
using relocationa_64 = basic_relocationa_64<elf64_basic_traits<Endian>>;

using relocationa_64le = relocationa_64<std::endian::little>;
using relocationa_64be = relocationa_64<std::endian::big>;
using relocationa_64ne = relocationa_64<std::endian::native>;


template <class Traits>
struct basic_program_header_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_program_header_32<typename Traits::byte_swapped_type>;

    endian<SegmentType>     type                = SegmentType::Null;
    Traits::offset_t        offset              = 0;
    Traits::address_t       virtual_address     = 0;
    Traits::address_t       physical_address    = 0;
    Traits::word_t          file_size           = 0;
    Traits::word_t          memory_size         = 0;
    Traits::word_t          flags               = 0;
    Traits::word_t          alignment           = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            offset,
            virtual_address,
            physical_address,
            file_size,
            memory_size,
            flags,
            alignment,
        };
    }
};

template <std::endian Endian>
using program_header_32 = basic_program_header_32<elf32_basic_traits<Endian>>;

using program_header_32le = program_header_32<std::endian::little>;
using program_header_32be = program_header_32<std::endian::big>;
using program_header_32ne = program_header_32<std::endian::native>;


template <class Traits>
struct basic_program_header_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_program_header_64<typename Traits::byte_swapped_type>;

    endian<SegmentType>     type                = SegmentType::Null;
    Traits::word_t          flags               = 0;
    Traits::offset_t        offset              = 0;
    Traits::address_t       virtual_address     = 0;
    Traits::address_t       physical_address    = 0;
    Traits::xword_t         file_size           = 0;
    Traits::xword_t         memory_size         = 0;
    Traits::xword_t         alignment           = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            flags,
            offset,
            virtual_address,
            physical_address,
            file_size,
            memory_size,
            alignment,
        };
    }
};

template <std::endian Endian>
using program_header_64 = basic_program_header_64<elf64_basic_traits<Endian>>;

using program_header_64le = program_header_64<std::endian::little>;
using program_header_64be = program_header_64<std::endian::big>;
using program_header_64ne = program_header_64<std::endian::native>;


template <class Traits>
struct basic_dynamic_tag_32
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_dynamic_tag_32<typename Traits::byte_swapped_type>;    

    Traits::word_t          type            = static_cast<std::uint32_t>(DynamicTagType::Null);
    Traits::word_t          val_or_ptr      = 0;

    constexpr DynamicTagType getType() const noexcept
    {
        return static_cast<DynamicTagType>(type);
    }

    constexpr void setType(DynamicTagType t) noexcept
    {
        type = static_cast<std::uint32_t>(t);
    }

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            val_or_ptr,
        };
    }
};

template <std::endian Endian>
using dynamic_tag_32 = basic_dynamic_tag_32<elf32_basic_traits<Endian>>;

using dynamic_tag_32le = dynamic_tag_32<std::endian::little>;
using dynamic_tag_32be = dynamic_tag_32<std::endian::big>;
using dynamic_tag_32ne = dynamic_tag_32<std::endian::native>;


template <class Traits>
struct basic_dynamic_tag_64
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_dynamic_tag_64<typename Traits::byte_swapped_type>;

    Traits::xword_t         type            = static_cast<std::uint64_t>(DynamicTagType::Null);
    Traits::xword_t         val_or_ptr      = 0;

    constexpr DynamicTagType getType() const noexcept
    {
        return static_cast<DynamicTagType>(type);
    }

    constexpr void setType(DynamicTagType t) noexcept
    {
        type = static_cast<std::uint64_t>(t);
    }

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            type,
            val_or_ptr,
        };
    }
};

template <std::endian Endian>
using dynamic_tag_64 = basic_dynamic_tag_64<elf64_basic_traits<Endian>>;

using dynamic_tag_64le = dynamic_tag_64<std::endian::little>;
using dynamic_tag_64be = dynamic_tag_64<std::endian::big>;
using dynamic_tag_64ne = dynamic_tag_64<std::endian::native>;


struct VersionDefinition
{
    VersionTableRevision    table_version   = VersionTableRevision::Current;
    std::uint16_t           flags           = 0;
    std::uint16_t           index           = 0;
    std::uint16_t           aux_count       = 0;
    std::uint32_t           hash            = 0;
    std::uint32_t           aux_offset      = 0;
    std::uint32_t           next_offset     = 0;
};

struct VersionDefinitionAuxiliaryData
{
    std::uint32_t           name_offset     = 0;
    std::uint32_t           next_offset     = 0;
};

struct VersionRequired
{
    VersionTableRevision    table_version   = VersionTableRevision::Current;
    std::uint16_t           aux_count       = 0;
    std::uint32_t           filename_offset = 0;
    std::uint32_t           aux_offset      = 0;
    std::uint32_t           next_offset     = 0;
};

struct VersionRequiredAuxiliaryData
{
    std::uint32_t           hash            = 0;
    std::uint16_t           flags           = 0;
    std::uint16_t           other           = 0;
    std::uint32_t           name_offset     = 0;
    std::uint32_t           next_offset     = 0;
};


struct file_header_magic
{
    // Length of the file identification bytes.
    static constexpr std::size_t IdentLength = 16;

    // Offsets of various fields in the identification bytes.
    static constexpr std::size_t IdentMagic0        = 0;
    static constexpr std::size_t IdentMagic1        = 1;
    static constexpr std::size_t IdentMagic2        = 2;
    static constexpr std::size_t IdentMagic3        = 3;
    static constexpr std::size_t IdentClass         = 4;
    static constexpr std::size_t IdentData          = 5;
    static constexpr std::size_t IdentVersion       = 6;
    static constexpr std::size_t IdentOSABI         = 7;
    static constexpr std::size_t IdentOSABIVersion  = 8;
    static constexpr std::size_t IdentPadStart      = 9;
    static constexpr std::size_t IdentPadLength     = IdentLength - IdentPadStart;

    // Expected values of the identification "magic" bytes.
    static constexpr std::uint8_t Magic0            = 0x7f;
    static constexpr std::uint8_t Magic1            = 'E';
    static constexpr std::uint8_t Magic2            = 'L';
    static constexpr std::uint8_t Magic3            = 'F';

    std::array<std::uint8_t, IdentLength>   ident   = {};


    bool isMagicValid() const;
    FileClass getClass() const;
    ByteOrder getByteOrder() const;
    FormatVersion getVersion() const;
    OSABI getOSABI() const;
    std::uint8_t getOSABIVersion() const;

    void setMagic();
    void setClass(FileClass);
    void setByteOrder(ByteOrder);
    void setVersion(FormatVersion = FormatVersion::Current);
    void setOSABI(OSABI = OSABI::Generic);
    void setOSABIVersion(std::uint8_t);
    void setPadding();
};


template <class Traits>
struct basic_file_header_common
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_file_header_common<typename Traits::byte_swapped_type>;

    file_header_magic                       magic   = {};
    endian<ObjectType>                      type    = ObjectType::None;
    endian<ProcessorType>                   machine = ProcessorType::None;
    Traits::word_t                          version = static_cast<std::uint32_t>(FormatVersion::Current);

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            magic,
            type,
            machine,
            version,
        };
    }
};

template <std::endian Endian>
using file_header_common = basic_file_header_common<elf32_basic_traits<Endian>>;

using file_header_common_le = file_header_common<std::endian::little>;
using file_header_common_be = file_header_common<std::endian::big>;
using file_header_common_ne = file_header_common<std::endian::native>;


template <class Traits>
struct basic_file_header_32 : basic_file_header_common<Traits> 
{
    template <class T>
    using endian            = Traits::template endian<T>;

    using byte_swapped_type = basic_file_header_32<typename Traits::byte_swapped_type>;    

    Traits::address_t   entry_point                 = 0;
    Traits::offset_t    program_header_offset       = 0;
    Traits::offset_t    section_header_offset       = 0;
    Traits::word_t      flags                       = 0;
    Traits::half_t      file_header_size            = sizeof(basic_file_header_32);
    Traits::half_t      program_header_size         = sizeof(basic_program_header_32<Traits>);
    Traits::half_t      program_header_count        = 0;
    Traits::half_t      section_header_size         = sizeof(basic_section_header_32<Traits>);
    Traits::half_t      section_header_count        = 0;
    Traits::half_t      section_header_name_section = 0;

    constexpr byte_swapped_type byte_swap() const noexcept
    {
        return
        {
            { *this },
            entry_point,
            program_header_offset,
            flags,
            file_header_size,
            program_header_size,
            program_header_count,
            section_header_size,
            section_header_count,
            section_header_name_section,
        };
    }
};

template <std::endian Endian>
using file_header_32 = basic_file_header_32<elf32_basic_traits<Endian>>;

using file_header_32le = file_header_32<std::endian::little>;
using file_header_32be = file_header_32<std::endian::big>;
using file_header_32ne = file_header_32<std::endian::native>;

/*struct FileHeader64 : FileHeaderCommon
{
    std::uint64_t       entry_point                 = 0;
    std::uint64_t       program_header_offset       = 0;
    std::uint64_t       section_header_offset       = 0;
    std::uint32_t       flags                       = 0;
    std::uint16_t       file_header_size            = sizeof(FileHeader64);
    std::uint16_t       program_header_size         = sizeof(ProgramHeader64);
    std::uint16_t       program_header_count        = 0;
    std::uint16_t       section_header_size         = 0;//sizeof(SectionHeader64);
    std::uint16_t       section_header_count        = 0;
    std::uint16_t       section_header_name_section = 0;
};*/


} // namespace System::Executable::Image::ELF


#endif /* ifndef __SYSTEM_EXECUTABLE_IMAGE_ELF_TYPES_H */

#pragma once
#ifndef __SYSTEM_FIRMWARE_ARCH_X86_MPS_MPS_H
#define __SYSTEM_FIRMWARE_ARCH_X86_MPS_MPS_H


#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>


namespace System::Firmware::X86::MPS
{


// Versions of the Intel multiprocessor specification.
enum class revision_t : std::uint8_t
{
    MPS_1_1     = 0x01,     // Version 1.1.
    MPS_1_4     = 0x04,     // Version 1.4.
};

constexpr revision_t MPS_1_1 = revision_t::MPS_1_1;     // Version 1.1.
constexpr revision_t MPS_1_4 = revision_t::MPS_1_4;     // Version 1.4.

// Table entry types.
enum class entry_type_t : std::uint8_t
{
    // Base entry types from version 1.1 of the specification.
    Processor       = 0x00,     // Processor information.
    Bus             = 0x01,     // Bus information.
    IOAPIC          = 0x02,     // I/O APIC information.
    IOInterrupt     = 0x03,     // I/O APIC interrupt assignment.
    LocalInterrupt  = 0x04,     // Local APIC interrupt assignment.

    // Extended entry types from version 1.4 of the specification.
    AddressMapping  = 0x80,     // System address space mapping.
    BusHierarchy    = 0x81,     // Bus interconnection information.
    BusModifier     = 0x82,     // Compatibility bus address space modifier.
};

using EntryType = entry_type_t;

// Returns the length of the given entry.
constexpr std::size_t entrySize(entry_type_t type)
{
    switch (type)
    {
        case EntryType::Processor:
        case EntryType::AddressMapping:
            return 20;

        case EntryType::Bus:
        case EntryType::IOAPIC:
        case EntryType::IOInterrupt:
        case EntryType::LocalInterrupt:
        case EntryType::BusHierarchy:
        case EntryType::BusModifier:
            return 8;

        default:
            // Shouldn't happen.
            return 0;
    }
}


constexpr std::uint32_t FloatingPointerSignature = 0x5F504D5F;  // '_MP_'

struct floating_pointer_t
{
    std::uint32_t   signature;                  // Identification signature;
    std::uint32_t   address;                    // Physical address of the configuration table.
    std::uint8_t    length;                     // Length of this structure in units of 16 bytes.
    revision_t      revision;                   // Multiprocessor specification revision.
    std::uint8_t    checksum;                   // Checksum over this table.
    std::uint8_t    configuration_type;         // Pre-defined configuration type.
    std::uint32_t   :6;
    std::uint32_t   imcr_present:1;             // Set if IMCR I/O port is present.
    std::uint32_t   :24;
};

static_assert(sizeof(floating_pointer_t) == 16);

constexpr std::uint32_t ConfigurationSignature = 0x504D4350;    // 'PCMP'

struct configuration_table_t
{
    std::uint32_t           signature;          // Identification signature.
    std::uint16_t           base_length;        // Length of the base table.
    revision_t              revision;           // Multiprocessor specification version.
    std::uint8_t            checksum;           // Checksum over the base table.
    std::array<char, 8>     oem_id;             // OEM ID string (space padded).
    std::array<char, 12>    product_id;         // Product ID string (space padded).
    std::uint32_t           oem_table_address;  // Physical address of the OEM data table.
    std::uint16_t           oem_table_size;     // Size of the OEM data table.
    std::uint16_t           entry_count;        // Number of entries in the base portion of the table.
    std::uint32_t           lapic_address;      // Physical address Local APICs are mapped to.
    std::uint16_t           extended_length;    // Length of the extended portion of the table.
    std::uint8_t            extended_checksum;  // Checksum over the extended portion of the table.
    std::uint8_t            :8;
};

static_assert(sizeof(configuration_table_t) == 44);

// Common header for base-table entries.
struct entry_t
{
    entry_type_t            type;               // Entry type.

    constexpr std::size_t size() const;
    constexpr const entry_t* next() const;
};

template <EntryType Type>
struct fixed_entry_t : entry_t
{
    constexpr fixed_entry_t() : entry_t{.type = Type} {}
};

// Common header for extended-table entries.
struct extended_entry_t : entry_t
{
    std::uint8_t            length;             // Length of this entry.
};

template <EntryType Type>
struct dynamic_entry_t : extended_entry_t
{
    constexpr dynamic_entry_t() : entry_t{Type, entrySize(Type)} {}
};

// Describes a processor and its local APIC.
struct processor_entry_t : fixed_entry_t<EntryType::Processor>
{
    std::uint8_t            lapic_id;           // APIC ID of the processor's local APIC.
    std::uint8_t            lapic_version;      // LAPIC version information.
    std::uint8_t            enabled:1;          // Set if this processor is enabled.
    std::uint8_t            bootstrap:1;        // Set if this is the bootstrap processor.
    std::uint8_t            :6;
    std::uint32_t           signature;          // CPU version information (as per CPUID).
    std::uint32_t           feature_flags;      // CPU feature information (as per CPUID).
    std::uint32_t           :32;
    std::uint32_t           :32;
};

static_assert(sizeof(processor_entry_t) == entrySize(EntryType::Processor));

// Describes a bus.
struct bus_entry_t : fixed_entry_t<EntryType::Bus>
{
    std::uint8_t            bus_id;             // Unique ID for this bus.
    std::array<char, 6>     bus_type;           // Mnemonic bus type.
};

// Registered bus types.
using bus_type_t = std::array<char, 6>;
namespace BusType
{

template <const char S[7]>
constexpr bus_type_t BusType = {S[0], S[1], S[2], S[3], S[4], S[5]};

constexpr char CBusStr[]        = "CBUS  ";
constexpr char CBusIIStr[]      = "CBUSII";
constexpr char EISAStr[]        = "EISA  ";
constexpr char FutureStr[]      = "FUTURE";
constexpr char InternalStr[]    = "INTERN";
constexpr char ISAStr[]         = "ISA   ";
constexpr char MultibusIStr[]   = "MBI   ";
constexpr char MultibusIIStr[]  = "MBII  ";
constexpr char MicrochannelStr[]= "MCA   ";
constexpr char MPIStr[]         = "MPI   ";
constexpr char MPSAStr[]        = "MPSA  ";
constexpr char NubusStr[]       = "NUBUS ";
constexpr char PCIStr[]         = "PCI   ";
constexpr char PCMCIAStr[]      = "PCMCIA";
constexpr char TurboChannelStr[]= "TC    ";
constexpr char VesaLocalBusStr[]= "VL    ";
constexpr char VMEbusStr[]      = "VME   ";
constexpr char ExpressStr[]     = "XPRESS";

constexpr bus_type_t CBus           = BusType<CBusStr>;
constexpr bus_type_t CBusII         = BusType<CBusIIStr>;
constexpr bus_type_t EISA           = BusType<EISAStr>;
constexpr bus_type_t FutureBus      = BusType<FutureStr>;
constexpr bus_type_t Internal       = BusType<InternalStr>;
constexpr bus_type_t ISA            = BusType<ISAStr>;
constexpr bus_type_t MultibusI      = BusType<MultibusIStr>;
constexpr bus_type_t MultibusII     = BusType<MultibusIIStr>;
constexpr bus_type_t Microchannel   = BusType<MicrochannelStr>;
constexpr bus_type_t MPI            = BusType<MPIStr>;
constexpr bus_type_t MPSA           = BusType<MPSAStr>;
constexpr bus_type_t Nubus          = BusType<NubusStr>;
constexpr bus_type_t PCI            = BusType<PCIStr>;
constexpr bus_type_t PCMCIA         = BusType<PCMCIAStr>;
constexpr bus_type_t TurboChannel   = BusType<TurboChannelStr>;
constexpr bus_type_t VesaLocalBus   = BusType<VesaLocalBusStr>;
constexpr bus_type_t VMEbus         = BusType<VMEbusStr>;
constexpr bus_type_t Express        = BusType<ExpressStr>;

};

static_assert(sizeof(bus_entry_t) == entrySize(EntryType::Bus));

// Describes an I/O APIC.
struct ioapic_entry_t : fixed_entry_t<EntryType::IOAPIC>
{
    std::uint8_t    ioapic_id;          // APIC ID of this I/O APIC.
    std::uint8_t    ioapic_version;     // I/O APIC version information.
    std::uint8_t    enabled:1;          // Set if this I/O APIC is enabled.
    std::uint8_t    :7;
    std::uint32_t   ioapic_address;     // Physical address of the I/O APIC's registers.
};

static_assert(sizeof(ioapic_entry_t) == entrySize(EntryType::IOAPIC));

// Interrupt types (i.e how the APIC should treat them).
enum class interrupt_type_t : std::uint8_t
{
    Int         = 0x00,     // Normal vectored interrupt via APIC redirection table.
    NMI         = 0x01,     // Non-maskable interrupt.
    SMI         = 0x02,     // System management interrupt.
    ExtInt      = 0x03,     // Vectored interrupt with vector supplied by an external PIC.
};

using InterruptType = interrupt_type_t;

// Interrupt polarities.
namespace Polarity
{
constexpr std::uint16_t Bus         = 0b00;     // Polarity follows bus specifications.
constexpr std::uint16_t ActiveHigh  = 0b01;     // Asserted at high volrage.
constexpr std::uint16_t ActiveLow   = 0b11;     // Asserted at low voltage.
}

// Interrupt trigger modes.
namespace Trigger
{
constexpr std::uint16_t Bus         = 0b00;     // Trigger mode follows bus specifications.
constexpr std::uint16_t Edge        = 0b01;     // Triggered by a high-low (or low-high) transition.
constexpr std::uint16_t Level       = 0b11;     // Triggered by a high (or low) voltage.
}

// Describes connections to I/O APIC interrupt pins.
struct io_interrupt_entry_t : fixed_entry_t<EntryType::IOInterrupt>
{
    interrupt_type_t    interrupt_type;     // Interrupt type.
    std::uint16_t       polarity:2;         // Interrupt polarity.
    std::uint16_t       trigger_mode:2;     // Interrupt trigger mode.
    std::uint16_t       :12;
    std::uint8_t        source_bus;         // ID of the source bus.
    std::uint8_t        source_irq;         // Interrupt index on the source bus.
    std::uint8_t        target_ioapic;      // ID of the target I/O APIC.
    std::uint8_t        target_irq;         // Interrupt index on the target I/O APIC.
};

static_assert(sizeof(io_interrupt_entry_t) == entrySize(EntryType::IOInterrupt));

// Describes connections to local APIC interrupt pins.
struct local_interrupt_entry_t : entry_t
{
    interrupt_type_t    interrupt_type;     // Interrupt type.
    std::uint16_t       polarity:2;         // Interrupt polarity.
    std::uint16_t       trigger_mode:2;     // Interrupt trigger mode.
    std::uint16_t       :12;
    std::uint8_t        source_bus;         // ID of the source bus.
    std::uint8_t        source_irq;         // Interrupt index on the source bus.
    std::uint8_t        target_lapic;       // ID of the target local APIC.
    std::uint8_t        target_irq;         // Interrupt index on teh target local APIC.
};

static_assert(sizeof(local_interrupt_entry_t) == entrySize(EntryType::LocalInterrupt));

// Types of addresses that buses can claim.
enum class address_type_t : std::uint8_t
{
    IO          = 0x00,     // I/O port addresses.
    Memory      = 0x01,     // Normal memory addresses.
    Prefetch    = 0x02,     // ??? (prefetchable memory?).
};

using AddressType = address_type_t;

// Describes what addresses are claimed by a bus.
struct [[gnu::packed, gnu::aligned(4)]] address_mapping_entry_t : extended_entry_t
{
    std::uint8_t        bus_id;             // ID of the bus with this mapping.
    address_type_t      address_type;       // Type of address being mapped.
    std::uint64_t       address_base;       // Base address of the mapping.
    std::uint64_t       address_length;     // Length of the mapping.
};

static_assert(sizeof(address_mapping_entry_t) == entrySize(EntryType::AddressMapping));

// Describes how one bus relates to its parent bus.
struct bus_hierarchy_entry_t : extended_entry_t
{
    std::uint8_t        bus_id;                 // ID of the bus being described.
    std::uint8_t        subtractive_decode:1;   // Bus claims all addresses not claimed by other devices.
    std::uint8_t        :7;
    std::uint8_t        parent_bus;             // ID of the parent bus.
    std::uint8_t        :8;
    std::uint16_t       :16;
};

static_assert(sizeof(bus_hierarchy_entry_t) == entrySize(EntryType::BusHierarchy));

// Predefined address range IDs.
enum class predefined_range_t : std::uint32_t
{
    ISA         = 0x00,         // ISA I/O ports.
    VGA         = 0x01,         // VGA I/O ports.
};

using PredefinedRange = predefined_range_t;

// Describes pre-defined (legacy ISA) address range mappings in a concise manner.
struct bus_modifier_entry_t : extended_entry_t
{
    std::uint8_t        bus_id;                 // ID of the bus being modified.
    std::uint8_t        subtract:1;             // If set, predefined ranges are to be subtracted (otherwise, added).
    std::uint8_t        :7;
    predefined_range_t  range;                  // Ranges of addresses to add (or subtract).
};


constexpr std::size_t entry_t::size() const
{
    // Size is the static size if a base entry type.
    if (static_cast<std::uint8_t>(type) <= static_cast<std::uint8_t>(EntryType::LocalInterrupt))
        return entrySize(type);
    else
        return static_cast<const extended_entry_t*>(this)->length;
}

constexpr const entry_t* entry_t::next() const
{
    // Do pointer arithmetic at run-time or casting at compile-time.
    if (std::is_constant_evaluated())
    {
        switch (type)
        {
            case EntryType::Processor:
                return static_cast<const processor_entry_t*>(this) + 1;
            
            case EntryType::Bus:
                return static_cast<const bus_entry_t*>(this) + 1;

            case EntryType::IOAPIC:
                return static_cast<const ioapic_entry_t*>(this) + 1;

            case EntryType::IOInterrupt:
                return static_cast<const io_interrupt_entry_t*>(this) + 1;

            case EntryType::LocalInterrupt:
                return static_cast<const local_interrupt_entry_t*>(this) + 1;

            case EntryType::AddressMapping:
                return static_cast<const address_mapping_entry_t*>(this) + 1;

            case EntryType::BusHierarchy:
                return static_cast<const bus_hierarchy_entry_t*>(this) + 1;

            case EntryType::BusModifier:
                return static_cast<const bus_modifier_entry_t*>(this) + 1;
        }
    }

    auto addr = reinterpret_cast<const std::byte*>(this);
    return reinterpret_cast<const entry_t*>(addr + size());
}


const configuration_table_t* getPredefinedConfiguration(std::uint8_t id);


class entry_iterator_t
{
public:

    using difference_type           = std::ptrdiff_t;
    using value_type                = entry_t;
    using pointer                   = const entry_t*;
    using reference                 = const entry_t&;
    using iterator_category         = std::forward_iterator_tag;

    constexpr entry_iterator_t() = default;
    constexpr entry_iterator_t(const entry_iterator_t&) = default;
    constexpr entry_iterator_t(entry_iterator_t&&) = default;
    constexpr entry_iterator_t& operator=(const entry_iterator_t&) = default;
    constexpr entry_iterator_t& operator=(entry_iterator_t&&) = default;
    ~entry_iterator_t() = default;

    explicit entry_iterator_t(const configuration_table_t& conf)
        : m_ptr(reinterpret_cast<const entry_t*>(&conf + 1)),
          m_index(0)
    {
    }

    explicit entry_iterator_t(const entry_t* end, std::uint16_t end_index)
        : m_ptr(end),
          m_index(end_index)
    {
    }

    constexpr entry_iterator_t& operator++()
    {
        m_ptr = m_ptr->next();
        ++m_index;
        return *this;
    }

    constexpr entry_iterator_t operator++(int)
    {
        auto copy = *this;
        operator++();
        return copy;
    }

    constexpr const entry_t& operator*() const
    {
        return *m_ptr;
    }

    constexpr const entry_t* operator->() const
    {
        return m_ptr;
    }

private:

    const entry_t*                  m_ptr = nullptr;
    std::uint16_t                   m_index = 0;
};


class ConfigurationTable
    : private configuration_table_t
{
public:

    using iterator          = entry_iterator_t;
    using const_iterator    = entry_iterator_t;

    // Not a real object; only a wrapper around a data table.
    ConfigurationTable() = delete;
    ConfigurationTable(const ConfigurationTable&) = delete;
    ConfigurationTable(ConfigurationTable&&) = delete;
    ConfigurationTable& operator=(const ConfigurationTable&) = delete;
    ConfigurationTable& operator=(ConfigurationTable&&) = delete;
    ~ConfigurationTable() = delete;

    constexpr const configuration_table_t& table() const
    {
        return *this;
    }

    const_iterator begin() const
    {
        return entry_iterator_t{table()};
    }

    const_iterator end() const
    {
        auto ptr = reinterpret_cast<const std::byte*>(&table());
        auto past_the_end = reinterpret_cast<const entry_t*>(ptr + table().base_length + table().extended_length);
        return entry_iterator_t{past_the_end, table().entry_count};
    }


    // Wraps the given raw configuration table without performing any validation checks.
    static const ConfigurationTable* wrapUnchecked(const configuration_table_t* table)
    {
        return static_cast<const ConfigurationTable*>(table);
    }

    // Wraps the given raw configuration table and returns a validated pointer.
    //
    // If the table isn't valid (for example, incorrect checksum or signature), nullptr will be returned.
    static const ConfigurationTable* wrap(const configuration_table_t* table)
    {
        auto wrapped = wrapUnchecked(table);
        if (!wrapped->validate())
            return nullptr;

        return wrapped;
    }

private:

    bool checkSignature() const;
    bool checkHeader() const;
    bool checkSum() const;
    
    bool validate() const
    {
        return checkSignature() && checkHeader() && checkSum();
    }
};


class FloatingPointer
    : private floating_pointer_t
{
public:

    // Not a real object; only a wrapper around a data table.
    // TODO: ctors, dtors, etc.

    constexpr const floating_pointer_t& table() const
    {
        return *this;
    }

    constexpr std::uint32_t address() const
    {
        return table().address;
    }

    constexpr std::size_t length() const
    {
        return std::size_t(table().length) * 16;
    }

    constexpr revision_t revision() const
    {
        return table().revision;
    }

    constexpr std::uint8_t configurationType() const
    {
        return table().configuration_type;
    }

    constexpr bool isIMCRPresent() const
    {
        return table().imcr_present;
    }


    static const FloatingPointer* wrapUnchecked(const floating_pointer_t* table)
    {
        return static_cast<const FloatingPointer*>(table);
    }

    static const FloatingPointer* wrap(const floating_pointer_t* table)
    {
        auto wrapped = wrapUnchecked(table);
        if (!wrapped->validate())
            return nullptr;

        return wrapped;
    }

    static const FloatingPointer* searchMemory(const void*, std::size_t);

private:

    bool checkSignature() const;
    bool checkFields() const;
    bool checkSum() const;

    bool validate() const
    {
        return checkSignature() && checkFields() && checkSum();
    }
};


} // namespace System::Firmware::X86::MPS


#endif /* ifndef __SYSTEM_FIRMWARE_ARCH_X86_MPS_MPS_H */

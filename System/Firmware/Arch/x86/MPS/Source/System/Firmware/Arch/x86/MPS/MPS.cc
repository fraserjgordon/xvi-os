#include <System/Firmware/Arch/x86/MPS/MPS.hh>


namespace System::Firmware::X86::MPS
{


// GCC doesn't like the structure initializers in this section...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

// Utility method that generates an I/O interrupt entry for the given IRQ.
constexpr auto createIRQEntry(std::uint8_t from, std::uint8_t to)
{
    return io_interrupt_entry_t
    {
        .interrupt_type = InterruptType::Int,
        .polarity = Polarity::Bus,
        .trigger_mode = Trigger::Bus,
        .source_bus = 0,
        .source_irq = from,
        .target_ioapic = 2,
        .target_irq = to,
    };
};

// Utility method that generates an I/O interrupt entry for an 8259-equivalent PIC.
constexpr auto createLegacyPICEntry()
{
    return io_interrupt_entry_t
    {
        .interrupt_type = InterruptType::ExtInt,
        .polarity = 0,
        .trigger_mode = 0,
        .source_bus = 0,
        .source_irq = 0,
        .target_ioapic = 2,
        .target_irq = 0,
    };
}

// Utility method for finalising predefined configuration definitions.
template <class... Entries>
constexpr auto
createFixedConfiguration(std::uint8_t id, Entries&&... entries)
{   
    auto id_char = static_cast<char>('0' + id);
    
    // TODO: calculate lengths, checksums, etc.
    auto table = std::make_tuple
    (
        // The configuration table.
        configuration_table_t
        {
            .signature = ConfigurationSignature,
            .base_length = 0,
            .revision = MPS_1_1,
            .checksum = 0,
            .oem_id = {'%','P','r','e','-','D','e','f'},
            .product_id = {'%','D','e','f','a','u','l','t',' ','#',id_char,' ',},
            .oem_table_address = 0,
            .oem_table_size = 0,
            .entry_count = 0,
            .lapic_address = 0xFEE00000,
            .extended_length = 0,
            .extended_checksum = 0,
        },
        
        // Standard entries.
        //
        // IRQ0 is routed to I/O Int#2 in most (but not all) pre-defined configurations.
        // Not all pre-defined configurations have identity mappings for IRQ13.
        processor_entry_t
        {
            .lapic_id = 0,
            .lapic_version = 0x00,
            .enabled = true,
            .bootstrap = true,
            .signature = 0,
            .feature_flags = 0,
        },
        processor_entry_t
        {
            .lapic_id = 1,
            .lapic_version = 0x00,
            .enabled = true,
            .bootstrap = false,
            .signature = 0,
            .feature_flags = 0,
        },
        ioapic_entry_t
        {
            .ioapic_id = 2,
            .ioapic_version = 0,
            .enabled = true,
            .ioapic_address = 0xFEC00000,
        },
        local_interrupt_entry_t
        {
            .interrupt_type = InterruptType::ExtInt,
            .polarity = 0,
            .trigger_mode = 0,
            .source_bus = 0,
            .source_irq = 0,
            .target_lapic = 0,
            .target_irq = 0,
        },
        local_interrupt_entry_t
        {
            .interrupt_type = InterruptType::ExtInt,
            .polarity = 0,
            .trigger_mode = 0,
            .source_bus = 0,
            .source_irq = 0,
            .target_lapic = 1,
            .target_irq = 0,
        },
        local_interrupt_entry_t
        {
            .interrupt_type = InterruptType::NMI,
            .polarity = 0,
            .trigger_mode = 0,
            .source_bus = 0,
            .source_irq = 0,
            .target_lapic = 0,
            .target_irq = 1,
        },
        local_interrupt_entry_t
        {
            .interrupt_type = InterruptType::NMI,
            .polarity = 0,
            .trigger_mode = 0,
            .source_bus = 0,
            .source_irq = 0,
            .target_lapic = 1,
            .target_irq = 1,
        },
        createIRQEntry(1, 1),
        createIRQEntry(3, 3),
        createIRQEntry(4, 4),
        createIRQEntry(5, 5),
        createIRQEntry(6, 6),
        createIRQEntry(7, 7),
        createIRQEntry(8, 8),
        createIRQEntry(9, 9),
        createIRQEntry(10, 10),
        createIRQEntry(11, 11),
        createIRQEntry(12, 12),
        createIRQEntry(14, 14),
        createIRQEntry(15, 15),

        // Custom entries.
        std::forward<Entries>(entries)...
    );

    // Fill in the table size information fields.
    auto& conf = std::get<0>(table);
    conf.entry_count = std::tuple_size_v<decltype(table)> - 1;
    conf.base_length = sizeof(table) - sizeof(configuration_table_t);

    return table;
}

// Predefined configuration #1
constexpr auto PredefinedConfiguration1 = createFixedConfiguration
(
    1,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::ISA,
    },
    createLegacyPICEntry(),
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);

// Predefined configuration #2
constexpr auto PredefinedConfiguration2 = createFixedConfiguration
(
    2,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::EISA,
    },
    createLegacyPICEntry()
);

// Predefined configuration #3
constexpr auto PredefinedConfiguration3 = createFixedConfiguration
(
    3,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::EISA,
    },
    createLegacyPICEntry(),
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);

// Predefined configuration #4
constexpr auto PredefinedConfiguration4 = createFixedConfiguration
(
    4,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::Microchannel,
    },
    createLegacyPICEntry(),
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);

// Predefined configuration #5
constexpr auto PredefinedConfiguration5 = createFixedConfiguration
(
    5,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::ISA,
    },
    bus_entry_t
    {
        .bus_id = 1,
        .bus_type = BusType::PCI,
    },
    createLegacyPICEntry(),
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);

// Predefined configuration #6
constexpr auto PredefinedConfiguration6 = createFixedConfiguration
(
    6,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::EISA,
    },
    bus_entry_t
    {
        .bus_id = 1,
        .bus_type = BusType::PCI,
    },
    createLegacyPICEntry(),
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);

// Predefined configuration #7
constexpr auto PredefinedConfiguration7 = createFixedConfiguration
(
    7,
    bus_entry_t
    {
        .bus_id = 0,
        .bus_type = BusType::Microchannel,
    },
    bus_entry_t
    {
        .bus_id = 1,
        .bus_type = BusType::PCI,
    },
    createIRQEntry(0, 2),
    createIRQEntry(13, 13)
);


#pragma GCC diagnostic pop


constexpr const configuration_table_t* PredefinedConfigurations[7] =
{
    &std::get<0>(PredefinedConfiguration1),
    &std::get<0>(PredefinedConfiguration2),
    &std::get<0>(PredefinedConfiguration3),
    &std::get<0>(PredefinedConfiguration4),
    &std::get<0>(PredefinedConfiguration5),
    &std::get<0>(PredefinedConfiguration6),
    &std::get<0>(PredefinedConfiguration7),
};


const configuration_table_t* getPredefinedConfiguration(std::uint8_t id)
{
    if (id < 1 || id > 7)
        return nullptr;

    return PredefinedConfigurations[id];
}


const FloatingPointer* FloatingPointer::searchMemory(const void* memory, std::size_t length)
{
    // Align the pointer to the alignment specified in the MPS documents: 16 bytes.
    auto address = reinterpret_cast<std::uintptr_t>(memory);
    auto aligned_address = (address + 15) & ~std::uintptr_t(15);
    auto diff = aligned_address - address;
    length -= diff;
    memory = reinterpret_cast<const void*>(aligned_address);

    while (length >= sizeof(FloatingPointer))
    {
        // Check if we have a valid floating pointer table at this address.
        auto ptr = reinterpret_cast<const floating_pointer_t*>(memory);
        auto wrapped = wrap(ptr);
        if (wrapped != nullptr)
            return wrapped;

        // Try the next 16-byte-aligned address.
        length -= 16;
        memory = reinterpret_cast<const void*>(reinterpret_cast<const std::byte*>(memory) + 16);
    }

    // Couldn't find a valid floating pointer table.
    return nullptr;
}

bool FloatingPointer::checkSignature() const
{
    return table().signature == FloatingPointerSignature;
}

bool FloatingPointer::checkFields() const
{
    // Check that the length (in units of 16 bytes) matches what we expect.
    if (table().length * 16 != sizeof(floating_pointer_t))
        return false;

    // Check that we understand the table version. The standard doesn't give any explicit statements about compatibility
    // so we reject any unknown versions.
    auto revision = table().revision;
    if (revision != MPS_1_1 && revision != MPS_1_4)
        return false;

    // Check that the configuration type is recognised. Valid values are:
    //  - 0     (defined via a configuration table).
    //  - 1...7 (pre-defined configuration).
    if (table().configuration_type > 7)
        return false;

    // All of the field checks passed.
    return true;
}

bool FloatingPointer::checkSum() const
{
    // Check that the whole of the table sums to zero.
    auto ptr = reinterpret_cast<const std::uint8_t*>(&table());
    auto length = table().length * 16U;
    std::uint8_t sum = 0;
    for (std::size_t i = 0; i < length; ++i)
        sum += ptr[i];

    return (sum == 0);
}


bool ConfigurationTable::checkSignature() const
{
    return table().signature == ConfigurationSignature;
}

bool ConfigurationTable::checkHeader() const
{
    // Check that the length covers at least the configuration table header.
    if (table().base_length < sizeof(configuration_table_t))
        return false;

    // Check that we understand the table version. The standard doesn't give any explicit statements about compatibility
    // so we reject all unknown versions.
    auto revision = table().revision;
    if (revision != MPS_1_1 && revision != MPS_1_4)
        return false;

    // All of the header checks passed.
    return true;
}

bool ConfigurationTable::checkSum() const
{
    // Check that the base portion of the table sums to zero.
    auto ptr = reinterpret_cast<const std::uint8_t*>(&table());
    std::uint8_t sum = 0;
    for (std::size_t i = 0; i < table().base_length; ++i)
        sum += ptr[i];

    if (sum != 0)
        return false;

    // Check that the extended portion of the table also sums to zero, if it exists.
    if (table().revision == MPS_1_4)
    {
        for (std::size_t i = 0; i < table().extended_length; ++i)
            sum += ptr[i + table().base_length];

        if (sum != 0)
            return false;
    }

    // All of the checksums were valid.
    return true;
}


} // namespace System::Firmware::X86::MPS

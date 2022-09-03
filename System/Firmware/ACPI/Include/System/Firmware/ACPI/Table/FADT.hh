#ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_FADT_H
#define __SYSTEM_FIRMWARE_ACPI_TABLE_FADT_H


#include <System/Firmware/ACPI/Table/Common.hh>


namespace System::Firmware::ACPI
{


using fixed_feature_flag_t = std::uint32_t;

namespace FixedFeatureFlag
{
    constexpr fixed_feature_flag_t WBINVD           = 0x00000001;
    constexpr fixed_feature_flag_t WBINVDFlush      = 0x00000002;
    constexpr fixed_feature_flag_t CPUC1            = 0x00000004;
    constexpr fixed_feature_flag_t MPC2             = 0x00000008;
    constexpr fixed_feature_flag_t PowerButton      = 0x00000010;
    constexpr fixed_feature_flag_t SleepButton      = 0x00000020;
}


struct fadt
{
    static constexpr std::array<char, 4> Signature = { 'F', 'A', 'C', 'P' };

    data_table_header           header;
    uint32le_t                  firmware_ctrl;
    uint32le_t                  dsdt;
    std::uint8_t                interrupt_model;
    std::uint8_t                power_management_profile;
    uint16le_t                  sci_interrupt;
    uint32le_t                  smi_command_port;
    std::uint8_t                acpi_enable_command;
    std::uint8_t                acpi_disable_command;
    std::uint8_t                bios_s4_command;
    std::uint8_t                pstate_control_command;
    uint32le_t                  pm1a_event_block_address;
    uint32le_t                  pm1b_event_block_address;
    uint32le_t                  pm1a_control_block_address;
    uint32le_t                  pm1b_control_block_address;
    uint32le_t                  pm2_control_block_address;
    uint32le_t                  pmtimer_block_address;
    uint32le_t                  gpe0_block_address;
    uint32le_t                  gpe1_block_address;
    std::uint8_t                pm1_event_block_length;
    std::uint8_t                pm1_control_block_length;
    std::uint8_t                pm2_control_block_length;
    std::uint8_t                pmtimer_block_length;
    std::uint8_t                gpe0_block_length;
    std::uint8_t                gpe1_block_length;
    std::uint8_t                gpe1_base;
    std::uint8_t                cstate_enable_command;
    uint16le_t                  c2_latency;
    uint16le_t                  c3_latency;
    uint16le_t                  cache_flush_size;
    uint16le_t                  cache_flush_stride;
    std::uint8_t                duty_offset;
    std::uint8_t                duty_width;
    std::uint8_t                rtc_cmos_day_alarm_offset;
    std::uint8_t                rtc_cmos_month_alarm_offset;
    std::uint8_t                rtc_cmos_century_offset;
    uint16le_t                  x86_boot_flags;
    std::uint8_t                _reserved_111;
    uint32le_t                  feature_flags;
    generic_address             reset_register;
    std::uint8_t                reset_value;
    uint16le_t                  arm_boot_flags;
    std::uint8_t                minor_version;
    uint64le_t                  firmware_ctrl_64;
    uint64le_t                  dsdt_64;
    generic_address             pm1a_event_block;
    generic_address             pm1b_event_block;
    generic_address             pm1a_control_block;
    generic_address             pm1b_control_block;
    generic_address             pm2_control_block;
    generic_address             pmtimer_block;
    generic_address             gpe0_block;
    generic_address             gpe1_block;
    generic_address             sleep_control_register;
    generic_address             sleep_status_register;
    std::array<char, 8>         hypervisor_id;
};


} // namespace System::Firmware::ACPI


#endif /* ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_FADT_H */

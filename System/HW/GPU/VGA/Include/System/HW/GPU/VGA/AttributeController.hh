#ifndef __SYSTEM_HW_GPU_VGA_ATTRIBUTECONTROLLER_H
#define __SYSTEM_HW_GPU_VGA_ATTRIBUTECONTROLLER_H


#include <cstdint>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>


namespace System::HW::GPU::VGA
{


class AttributeController
{
public:

    enum class Register : std::uint8_t
    {
        Palette0            = 0x00,
        Palette1            = 0x01,
        Palette2            = 0x02,
        Palette3            = 0x03,
        Palette4            = 0x04,
        Palette5            = 0x05,
        Palette6            = 0x06,
        Palette7            = 0x07,
        Palette8            = 0x08,
        Palette9            = 0x09,
        Palette10           = 0x0A,
        Palette11           = 0x0B,
        Palette12           = 0x0C,
        Palette13           = 0x0D,
        Palette14           = 0x0E,
        Palette15           = 0x0F,
        ModeControl         = 0x10,
        OverscanColour      = 0x11,
        ColourPlaneEnable   = 0x12,
        HorizontalPixelPan  = 0x13,
        ColourSelect        = 0x14,
    };

    // The address register contains some control bits.
    static constexpr std::uint8_t PaletteAddressSource  = 0x20;


    constexpr AttributeController(std::uint16_t address_port, std::uint16_t data_read_port, std::uint16_t data_write_port, std::uint16_t flip_flop_reset_port) :
        m_addressPort(address_port),
        m_dataWritePort(data_write_port),
        m_dataReadPort(data_read_port),
        m_flipFlopResetPort(flip_flop_reset_port)
    {
    }

    std::uint8_t readRegister(Register r)
    {
        resetAddressDataFlipFlop();
        System::HW::CPU::X86::outb(m_addressPort, PaletteAddressSource | static_cast<std::uint8_t>(r));
        return System::HW::CPU::X86::inb(m_dataReadPort);
    }

    void writeRegister(Register r, std::uint8_t value)
    {
        resetAddressDataFlipFlop();

        // If writing a palette register, a specific bit needs to be clear in the address register or set otherwise.
        bool palette_reg = (static_cast<std::uint8_t>(r) <= static_cast<std::uint8_t>(Register::Palette15));
        if (palette_reg)
            System::HW::CPU::X86::outb(m_addressPort, static_cast<std::uint8_t>(r));
        else
            System::HW::CPU::X86::outb(m_addressPort, PaletteAddressSource | static_cast<std::uint8_t>(r));

        System::HW::CPU::X86::outb(m_dataWritePort, value);

        if (palette_reg)
            System::HW::CPU::X86::outb(m_addressPort, PaletteAddressSource);
    }

private:

    std::uint16_t   m_addressPort           = 0;
    std::uint16_t   m_dataWritePort         = 0;
    std::uint16_t   m_dataReadPort          = 0;
    std::uint16_t   m_flipFlopResetPort     = 0;


    void resetAddressDataFlipFlop()
    {
        // Read from ISR0 (and discard the value) - this resets the address/data flip-flop.
        static_cast<void>(System::HW::CPU::X86::inb(m_flipFlopResetPort));
    }
};


} // namespace System::HW::GPU::VGA


#endif /* ifndef __SYSTEM_HW_GPU_VGA_ATTRIBUTECONTROLLER_H */

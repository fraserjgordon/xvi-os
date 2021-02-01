#ifndef __SYSTEM_HW_GPU_VGA_GRAPHICSCONTROLLER_H
#define __SYSTEM_HW_CPU_VGA_GRAPHICSCONTROLLER_H


#include <cstdint>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>


namespace System::HW::GPU::VGA
{


class GraphicsController
{
public:

    enum class Register : std::uint8_t
    {
        SetReset            = 0x00,
        EnableSetReset      = 0x01,
        ColourCompare       = 0x02,
        DataRotate          = 0x03,
        ReadMapSelect       = 0x04,
        GraphicsMode        = 0x05,
        MiscGraphics        = 0x06,
        ColourDontCare      = 0x07,
        BitMask             = 0x08,
    };

    // Misc graphics register bits.
    static constexpr std::uint8_t MiscGraphics_GraphicsMode     = 0x01;
    static constexpr std::uint8_t MiscGraphics_OddEven          = 0x02;
    static constexpr std::uint8_t MiscGraphics_MemoryMapMask    = 0x0c;
    static constexpr std::uint8_t MiscGraphics_MemoryMap_128kA0 = 0x00;
    static constexpr std::uint8_t MiscGraphics_MemoryMap_64kA0  = 0x04;
    static constexpr std::uint8_t MiscGraphics_MemoryMap_32kB0  = 0x08;
    static constexpr std::uint8_t MiscGraphics_MemoryMap_32kB8  = 0x0c;


    constexpr GraphicsController(std::uint16_t address_port, std::uint16_t data_port) :
        m_addressPort(address_port),
        m_dataPort(data_port)
    {
    }

    std::uint8_t readRegister(Register r)
    {
        System::HW::CPU::X86::outb(m_addressPort, static_cast<std::uint8_t>(r));
        return System::HW::CPU::X86::inb(m_dataPort);
    }

    void writeRegister(Register r, std::uint8_t u)
    {
        System::HW::CPU::X86::outb(m_addressPort, static_cast<std::uint8_t>(r));
        System::HW::CPU::X86::outb(m_dataPort, u);
    }

private:

    std::uint16_t   m_addressPort   = 0;
    std::uint16_t   m_dataPort      = 0;
};


} // namespace System::HW::GPU::VGA


#endif /* ifndef __SYSTEM_HW_GPU_VGA_GRAPHICSCONTROLLER_H */

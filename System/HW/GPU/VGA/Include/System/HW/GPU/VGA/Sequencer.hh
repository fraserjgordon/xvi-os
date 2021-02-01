#ifndef __SYSTEM_HW_GPU_VGA_SEQUENCER_H
#define __SYSTEM_HW_GPU_VGA_SEQUENCER_H


#include <cstdint>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>


namespace System::HW::GPU::VGA
{


class Sequencer
{
public:

    enum class Register : std::uint8_t
    {
        Reset           = 0x00,
        ClockingMode    = 0x01,
        MapMask         = 0x02,
        CharacterMap    = 0x03,
        MemoryMode      = 0x05,
    };

    // Reset register bits.
    static constexpr std::uint8_t Reset_AsynchronousReset   = 0x01;
    static constexpr std::uint8_t Reset_SynchronousReset    = 0x02;

    // Clocking mode register bits
    static constexpr std::uint8_t ClockingMode_8DotChars    = 0x01;
    static constexpr std::uint8_t ClockingMode_Shift2       = 0x04;
    static constexpr std::uint8_t ClockingMode_HalfDotClock = 0x08;
    static constexpr std::uint8_t ClockingMode_Shift4       = 0x10;
    static constexpr std::uint8_t ClockingMode_ScreenDisable= 0x11;

    // Map mask register bits.
    static constexpr std::uint8_t MapMask_EnablePlane0      = 0x01;
    static constexpr std::uint8_t MapMask_EnablePlane1      = 0x02;
    static constexpr std::uint8_t MapMask_EnablePlane2      = 0x04;
    static constexpr std::uint8_t MapMasp_EnablePlane3      = 0x08;

    // Character map register bits.
    static constexpr std::uint8_t CharacterMap_SetAMask     = 0x2C;
    static constexpr std::uint8_t CharacterMap_SetBMask     = 0x13;

    // Memory mode register bits.
    static constexpr std::uint8_t MemoryMode_ExtendedMemory = 0x02;
    static constexpr std::uint8_t MemoryMode_OddEvenDisable = 0x04;
    static constexpr std::uint8_t MemoryMode_Chain4         = 0x08;


    constexpr Sequencer(std::uint16_t address_port, std::uint16_t data_port) :
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

    std::uint16_t   m_addressPort;
    std::uint16_t   m_dataPort;
};


} // namespace System::HW::GPU::VGA


#endif /* ifndef __SYSTEM_HW_GPU_VGA_SEQUENCER_H */

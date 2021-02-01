#ifndef __SYSTEM_HW_GPU_VGA_CRTC_H
#define __SYSTEM_HW_GPU_VGA_CRTC_H


#include <cstdint>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>


namespace System::HW::GPU::VGA
{


class CRTC
{
public:

    enum class Register : std::uint8_t
    {
        HorizontalTotal             = 0x00,
        HorizontalDisplayEnableEnd  = 0x01,
        HorizontalBlankingStart     = 0x02,
        HorizontalBlankingEnd       = 0x03,
        HorizontalRetraceStart      = 0x04,
        HorizontalRetraceEnd        = 0x05,
        VerticalTotal               = 0x06,
        Overflow                    = 0x07,
        PresetRowScan               = 0x08,
        MaximumScanLineAddress      = 0x09,
        CursorStart                 = 0x0A,
        CursorEnd                   = 0x0B,
        StartAddressHigh            = 0x0C,
        StartAddressLow             = 0x0D,
        CursorLocationHigh          = 0x0E,
        CursorLocationLow           = 0x0F,
        VerticalRetraceStart        = 0x10,
        VerticalRetraceEnd          = 0x11,
        VerticalDisplayEnableEnd    = 0x12,
        Offset                      = 0x13,
        UnderlineLocation           = 0x14,
        VerticalBlankingStart       = 0x15,
        VerticalBlankingEnd         = 0x16,
        ModeControl                 = 0x17,
        LineCompare                 = 0x18,
    };

    struct DisplayParameters
    {
        std::uint16_t   horizontalTotal;            // Number of character clocks per scan line.
        std::uint8_t    horizontalDisplay;          // Number of displayed character blocks per scan line.
        std::uint8_t    horizontalBlankingStart;    // Character clock at which horizontal blanking starts.
        std::uint8_t    horizontalBlankingEnd;      // Character clock at which horizontal blanking ends.
        std::uint8_t    horizontalRetraceStart;     // Character clock at which horizontal retrace starts.
        std::uint8_t    horizontalRetraceEnd;       // Character clock at which horizontal retrace ends.
        
        //std::uint8_t    clockSkew;                  // Number of character clocks to skew the display enable.

        std::uint16_t   verticalTotal;              // Number of scan lines per frame.
        std::uint16_t   verticalDisplay;            // Number of displayed scan lines per frame.
        std::uint16_t   verticalBlankingStart;      // Scan line at which vertical blanking starts.
        std::uint16_t   verticalBlankingEnd;        // Scan line at which vertical blanking ends.
        std::uint16_t   verticalRetraceStart;       // Scan line at which vertical retrace starts.
        std::uint16_t   verticalRetraceEnd;         // Scan line at which vertical retrace ends.

    };

    struct ScanOutParameters
    {
        std::uint8_t    linesPerCharacter;          // Number of scan lines to generate per character row.
        std::uint8_t    charactersPerRow;           // Number of memory positions to advance when moving to a new row.
        std::uint8_t    clockDivisor;               // Divisor to use to generate memory address counter from character clock.
        std::uint8_t    addressShift;               // Number of bits to left-shift memory address counter to get an address.
    };

    struct ScrollParameters
    {
        std::uint8_t    scrollPels;     // Number of pixels to scroll within the first character row.
        std::uint16_t   startAddress;   // Starting address within VGA memory.

        std::uint16_t   lineCompare;    // Split-screen location.
    };

    struct CursorParameters
    {
        bool            enabled;        // Cursor hidden if not set.
        std::uint8_t    startLine;      // Starting scan line within a character row.
        std::uint8_t    endLine;        // Ending scan line within a character row.
        std::uint8_t    cursorSkew;     // Number of character clocks to delay cursor signal.
        std::uint8_t    underline;      // Scan line used for drawing an underline.
    };


    constexpr CRTC(std::uint16_t address_port, std::uint16_t data_port) :
        m_addressPort(address_port),
        m_dataPort(data_port)
    {
    }

    std::uint8_t readRegister(Register r) const
    {
        System::HW::CPU::X86::outb(m_addressPort, static_cast<std::uint8_t>(r));
        return System::HW::CPU::X86::inb(m_dataPort);
    }

    void writeRegister(Register r, std::uint8_t u)
    {
        System::HW::CPU::X86::outb(m_addressPort, static_cast<std::uint8_t>(r));
        System::HW::CPU::X86::outb(m_dataPort, u);
    }

    DisplayParameters getDisplayParameters() const noexcept;

    ScanOutParameters getScanOutParameters() const noexcept;

    void setStartAddress(std::uint16_t start) noexcept;

private:

    std::uint16_t   m_addressPort;
    std::uint16_t   m_dataPort;
};


} // namespace System::HW::GPU::VGA


#endif /* ifndef __SYSTEM_HW_GPU_VGA_CRTC_H */

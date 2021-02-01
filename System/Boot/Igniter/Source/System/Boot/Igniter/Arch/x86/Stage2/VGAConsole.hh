#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_VGACONSOLE_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_VGACONSOLE_H


#include <cstdint>
#include <string_view>

#include <System/HW/GPU/VGA/VGA.hh>
#include <System/Utility/Logger/Message.hh>


namespace System::Boot::Igniter
{


class VGAConsole
{
public:

    VGAConsole();


    bool valid() const;

    void showCursor(bool show = true);

    void hideCursor()
    {
        showCursor(false);
    }

private:

    // The VGA device being driven.
    System::HW::GPU::VGA::VGADevice m_vga = {};

    // Basic VGA information.
    std::uint16_t*  m_base          = nullptr;
    std::ptrdiff_t  m_memorySize    = 0;        // Measured in 16-bit words, not bytes.
    std::uint16_t   m_width         = 0;
    std::uint16_t   m_height        = 0;

    // Display information.
    std::ptrdiff_t  m_offset        = 0;
    std::ptrdiff_t  m_scroll        = 0;
    std::ptrdiff_t  m_cursor        = 0;
    bool            m_showCursor    = false;

    // Palette mappings.
    std::uint8_t    m_attrTrace     = 0x06; // Brown-on-black.
    std::uint8_t    m_attrDebug     = 0x08; // Grey-on-black.
    std::uint8_t    m_attrVerbose   = 0x07; // Light grey-on-black.
    std::uint8_t    m_attrInfo      = 0x0f; // White-on-black.
    std::uint8_t    m_attrNotice    = 0x03; // Cyan-on-black.
    std::uint8_t    m_attrWarning   = 0x0e; // Yellow-on-black.
    std::uint8_t    m_attrError     = 0x05; // Magenta-on-black.
    std::uint8_t    m_attrCritical  = 0x04; // Red-on-black.
    std::uint8_t    m_attrAlert     = 0x0d; // Bright red-on-black.
    std::uint8_t    m_attrEmergency = 0x4f; // White-on-red.


    void doOutput(std::uint8_t attr, std::string_view string);

    void newline(std::uint8_t attr);

    void scroll(int lines);

    void scrollToEnd();

    void clear(std::uint8_t attr);

    void processMessage(System::Utility::Logger::opaque_message msg);
};


} // namespace System::Boot::Igniter


#endif /* __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_VGACONSOLE_H */

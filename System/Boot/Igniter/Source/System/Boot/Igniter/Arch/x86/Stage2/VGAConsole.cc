#include <System/Boot/Igniter/Arch/x86/Stage2/VGAConsole.hh>

#include <System/Utility/Logger/Processor.hh>

#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>


namespace System::Boot::Igniter
{


namespace VGA = System::HW::GPU::VGA;


VGAConsole::VGAConsole()
{   
    log(priority::trace, "Starting VGA console initialisation");

    using Gfx = VGA::GraphicsController;
    using Seq = VGA::Sequencer;

    // Probe the general setup of the VGA adaptor. We'll use it if it's in a reasonable-looking text mode.
    auto& gfx = m_vga.graphicsController();
    auto gfx_reg = gfx.readRegister(Gfx::Register::MiscGraphics);
    if ((gfx_reg & Gfx::MiscGraphics_GraphicsMode) == 0)
    {
        // The VGA is in text mode; where is it's memory?
        std::uintptr_t base;
        std::size_t size;
        switch (gfx_reg & Gfx::MiscGraphics_MemoryMapMask)
        {
            case Gfx::MiscGraphics_MemoryMap_128kA0:
                base = 0xA0000;
                size = 0x20000;
                break;

            case Gfx::MiscGraphics_MemoryMap_64kA0:
                base = 0xA0000;
                size = 0x10000;
                break;

            case Gfx::MiscGraphics_MemoryMap_32kB0:
                base = 0xB0000;
                size = 0x08000;
                break;

            case Gfx::MiscGraphics_MemoryMap_32kB8:
                base = 0xB8000;
                size = 0x08000;
                break;
        }

        // If necessary, remap the VGA memory so that we have the maximum available.
        if (base != 0xA0000 || size != 0x20000)
        {
            log(priority::debug, "VGA: remapping memory from {:#x}+{:x} to {:#x}+{:x}", base, size, 0xA0000, 0x20000);

            // Ensure that extended memory is enabled and adjust the memory map.
            auto& seq = m_vga.sequencer();
            seq.writeRegister(Seq::Register::MemoryMode, seq.readRegister(Seq::Register::MemoryMode) | Seq::MemoryMode_ExtendedMemory);
            gfx.writeRegister(Gfx::Register::MiscGraphics, (gfx_reg & ~Gfx::MiscGraphics_MemoryMapMask) | Gfx::MiscGraphics_MemoryMap_128kA0);

            base = 0xA0000;
            size = 0x20000;
        }

        // Save the memory parameters.
        m_base = reinterpret_cast<std::uint16_t*>(base);
        m_memorySize = static_cast<std::ptrdiff_t>(size / sizeof(std::uint16_t));

        // Next, determine the screen dimensions from the CRTC.
        auto& crtc = m_vga.crtc();
        auto dop = crtc.getDisplayParameters();
        auto sop = crtc.getScanOutParameters();
        m_width = dop.horizontalDisplay;
        m_height = dop.verticalDisplay / sop.linesPerCharacter;

        // Adjust the memory size to contain a whole number of lines.
        auto orig_size = m_memorySize;
        m_memorySize -= (m_memorySize % m_width);

        log(priority::verbose, "VGA: display is {}x{} characters", m_width, m_height);
        log(priority::debug, "VGA: {} bytes character memory available, {} usable", orig_size, m_memorySize);
    }
    else
    {
        log(priority::warning, "VGA: not in text mode; will not be used");
    }
    
    clear(m_attrInfo);

    // If everything went smoothly, automatically register as a sink for log messages.
    if (valid())
    {
        log(priority::verbose, "VGA: configured; boot messages will be logged to screen");
        
        System::Utility::Logger::registerMessageProcessor([](void* console, System::Utility::Logger::opaque_message msg)
        {
            static_cast<VGAConsole*>(console)->processMessage(msg);
        }, this);

        // Pump any stored log messages so they can be printed.
        readLogBuffer([this](System::Utility::Logger::opaque_message msg)
        {
            processMessage(msg);
        });
    }

    log(priority::trace, "VGA console initialisation complete");
}

bool VGAConsole::valid() const
{
    return m_base != nullptr;
}

void VGAConsole::processMessage(System::Utility::Logger::opaque_message msg)
{
    System::Utility::Logger::message m(msg);

    auto p = m.priority();
    std::uint8_t attr = m_attrInfo;
    if (p < priority::debug)
        attr = m_attrTrace;
    else if (p < priority::verbose)
        attr = m_attrDebug;
    else if (p < priority::info)
        attr = m_attrVerbose;
    else if (p < priority::notice)
        attr = m_attrInfo;
    else if (p < priority::warning)
        attr = m_attrNotice;
    else if (p < priority::error)
        attr = m_attrWarning;
    else if (p < priority::critical)
        attr = m_attrError;
    else if (p < priority::alert)
        attr = m_attrCritical;
    else if (p < priority::emergency)
        attr = m_attrAlert;
    else
        attr = m_attrEmergency;

    doOutput(attr, m.msg());

    m.release();
}

void VGAConsole::doOutput(std::uint8_t attr, std::string_view string)
{
    // Copy each of the characters (excluding newlines) to the output.
    for (auto c : string)
    {
        if (c != '\n')
            m_base[m_offset++] = static_cast<std::uint8_t>(c) | (attr << 8);
        else
            newline(attr);

        // If we've gone past the end of the VGA memory, wrap to the beginning.
        //! @TODO: this incorrectly assumes that a whole number of lines fit in memory.
        if (m_offset >= m_memorySize)
            m_offset -= m_memorySize;
    }

    // Terminate all messages with a newline and update the scroll position, if needed.
    newline(attr);
    scrollToEnd();
}

void VGAConsole::newline(std::uint8_t attr)
{
    // Write blanks to the output until we advance to a new line.
    do
    {
        m_base[m_offset++] = (attr << 8);
    }
    while (m_offset % m_width != 0);
}

void VGAConsole::scroll(int lines)
{
    // Update the scroll position.
    m_scroll += (lines * m_width);
    if (m_scroll > m_memorySize)
        m_scroll -= m_memorySize;

    // Perform a hardware scroll.
    m_vga.crtc().setStartAddress(m_scroll);
}

void VGAConsole::scrollToEnd()
{
    // Has the output scrolled off the end of the screen?
    //
    // This comparison needs to account for the wrapping that happens at the end of the VGA memory area.
    auto screen_size = m_width * m_height;
    auto screen_end = m_scroll + screen_size;
    if (m_offset > screen_end || (m_offset < m_scroll && m_offset + m_memorySize > screen_end))
    {
        // The output position is not on the screen. Scroll until it is. We've ensured that the output offset is at the
        // beginning of a line so we can subtract the screen size from it to get the new scroll value.
        auto screen_start = m_offset - screen_size;
        if (screen_start < 0)
            screen_start += m_memorySize;
        auto scroll_count = (screen_start - m_scroll) / m_width;

        scroll(scroll_count);
    }
}

void VGAConsole::clear(std::uint8_t attr)
{
    for (std::ptrdiff_t offset = 0; offset < m_memorySize; ++offset)
        m_base[offset] = (attr << 8);
}


} // namespace System::Boot::Igniter

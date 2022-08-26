#include <System/Boot/Igniter/Arch/x86/Stage2/V86.hh>

#include <cstring>

#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>


namespace System::Boot::Igniter
{


// Symbols pointing to the real-mode helper functions.
extern const std::byte _REALMODE_TEXT_START asm("_REALMODE_TEXT_START");
extern const std::byte _REALMODE_TEXT_END asm("_REALMODE_TEXT_END");


// Page containing the relocated real-mode helper functions.
static std::uint32_t s_realModeHelperPage = 0;

// Page to use for the stack when executing real-mode code.
static std::uint32_t s_realModeStackPage = 0;


void prepareV86Mode()
{
    log(priority::debug, "Preparing V86 supervisor");

    // Allocate some real-mode memory and copy our helper functions there.
    s_realModeHelperPage = allocateEarlyRealModePage();
    log(priority::debug, "V86: copying real-mode helpers to {:#07x}", s_realModeHelperPage);
    std::memcpy(reinterpret_cast<void*>(s_realModeHelperPage), &_REALMODE_TEXT_START, &_REALMODE_TEXT_END-&_REALMODE_TEXT_START);

    // Allocate a real-mode stack too.
    s_realModeStackPage = allocateEarlyRealModePage();
    log(priority::debug, "V86: real-mode stack at {:#07x}+{:x}", s_realModeStackPage, 4096);
}


}

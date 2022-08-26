#include <System/Boot/Igniter/Arch/x86/Stage1/A20.hh>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/BIOSCall.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>


namespace System::Boot::Igniter
{


namespace X86 = System::HW::CPU::X86;


// These two addresses must be congruent mod 2**10.
constexpr std::uint32_t TestLower = 0x00000800;
constexpr std::uint32_t TestUpper = 0x00100800;


static volatile std::uint32_t* testPtr(std::uint32_t address)
{
    return reinterpret_cast<volatile std::uint32_t*>(address);
}


static bool isA20Enabled()
{
    // Are the two locations equal?
    if (*testPtr(TestLower) == *testPtr(TestUpper))
    {
        // They're equal. But it might be a coincidence. Modify one of them and test again.
        std::uint32_t value = *testPtr(TestLower);
        *testPtr(TestLower) = ~value;
        bool enabled = !(*testPtr(TestLower) == *testPtr(TestUpper));

        // Restore the previous value.
        *testPtr(TestLower) = value;

        return enabled;
    }

    // The two locations were different, the A20 line is enabled.
    return true;
}


static bool enableA20Fast()
{
    // Fast method for enabling the A20 line via PS/2 System Control Port A.
    constexpr std::uint16_t ControlPort = 0x0092;
    constexpr std::uint8_t A20Enable = 0x02;

    // Read the current value.
    auto value = X86::inb(ControlPort);

    // If the A20 enabled flag is already set, this port must be in use for something else...
    if (value & A20Enable)
    {
        log(priority::debug, "A20: \"fast A20\" failed - port 0x92 current value {:#x} invalid", value);
        return false;
    }

    // Set the A20 enable bit.
    X86::outb(ControlPort, value | A20Enable);

    // Re-test the A20 status to check that it actually worked.
    if (!isA20Enabled())
    {
        log(priority::debug, "A20: \"fast A20\" failed");
        return false;
    }

    return true;
}


static bool enableA20BIOS()
{
    // Send a command to the BIOS to enable A20.
    bios_call_params params;
    params.ax = 0x2401;
    BIOSCall(0x15, &params);

    // Did the command fail?
    if (params.eflags.bits.cf)
    {
        std::string_view reason = "unknown";
        if (params.ah == 0x01)
            reason = "keyboard controller locked";
        else if (params.ah == 0x86)
            reason = "not supported";

        log(priority::debug, "A20: BIOS failed to enable A20 - {} ({:#x})", reason, params.ah);
        return false;
    }

    // The command succeeded. But we shouldn't trust the BIOS. Check ourselves.
    if (!isA20Enabled())
    {
        log(priority::debug, "A20: BIOS reported A20 enabled but still disabled");
        return false;
    }

    return true;
}


void enableA20()
{
    // Check whether A20 is already enabled.
    if (isA20Enabled())
    {
        log(priority::debug, "A20: already enabled");
        return;
    }

    // Use the BIOS method first as it knows the right method. Assuming it's implemented!
    log(priority::debug, "A20: attempting to enable via BIOS");
    if (enableA20BIOS())
    {
        log(priority::debug, "A20: enabled");
        return;
    }

    // Use the "fast A20" method via the PS/2 System Control Port A.
    log(priority::debug, "A20: attempting to enable via \"fast A20\"");
    if (enableA20Fast())
    {
        log(priority::debug, "A20: enabled");
        return;
    }

    //! @todo: fallback via keyboard controller.
}


} // namespace System::Boot::Igniter

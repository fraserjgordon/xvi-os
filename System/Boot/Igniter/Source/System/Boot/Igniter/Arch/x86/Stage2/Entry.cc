#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>

#include <System/ABI/C++/Demangle.hh>
#include <System/ABI/C++/Exception.hh>
#include <System/ABI/C++/Unwinder.hh>
#include <System/ABI/TLS/TLS.hh>
#include <System/Firmware/EFI/API.hh>
#include <System/Firmware/EFI/Protocols.hh>
#include <System/Firmware/EFI/Tables.hh>
#include <System/HW/CPU/Arch/x86/IO/IO.hh>

#include <System/Boot/Igniter/Multiboot/v1.hh>
#include <System/Boot/Igniter/Multiboot/v2.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Interrupts.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/MMU.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/V86.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/VGAConsole.hh>


namespace Logger = System::Utility::Logger;
namespace VGA = System::HW::GPU::VGA;
namespace X86 = System::HW::CPU::X86;

using namespace System::Firmware::EFI;


namespace System::Boot::Igniter
{


// Forward declaration of the main entry function.
void run();

// Uninitialised memory reserved for startup logging.
static std::array<std::byte, 4096> s_logBuffer;


// Symbols from the link script that are needed to set up exception handling.
extern "C"
{
extern const std::byte _TEXT_START[] asm("_TEXT_START");
extern const std::byte _DATA_START[] asm("_DATA_START");
extern const std::byte _EH_FRAME_HDR[] asm("_EH_FRAME_HDR");
extern const std::byte _TDATA_START[] asm("_TDATA_START");
extern const std::byte _TDATA_END[] asm("_TDATA_END");
extern const std::byte _TBSS_END[] asm("_TBSS_END");

extern std::byte g_initThreadTLS[];
}


// These functions require C linkage as they are referred to from assembly code or the link script.
extern "C"
{

void multibootEntry(std::uint32_t magic, const void* info);

EFIAPI EFI_STATUS _IgniterStage2EfiEntry(EFI_HANDLE, EFI_SYSTEM_TABLE*);
EFIAPI EFI_STATUS _IgniterStage2Efi32Entry(EFI_HANDLE, EFI_SYSTEM_TABLE*);
EFIAPI EFI_STATUS _IgniterStage2Efi64Entry(EFI_HANDLE, EFI_SYSTEM_TABLE*);

// REMOVE ME
void waitOnAddress() asm("Syscall.Sync.WaitOnAddress");
void notifyAddress() asm("Syscall.Sync.NotifyAddress");
void waitOnAddress() {}
void notifyAddress() {}


} // extern "C"


void multibootEntry(std::uint32_t magic, const void* info)
{
    // Check the magic number to see whether a v1 or v2 loader was used.
    switch (magic)
    {
        case MultibootV1LoaderMagic:
            setMultibootTablePointer(reinterpret_cast<const multiboot_v1_info*>(info));
            break;

        case MultibootV2LoaderMagic:
            setMultibootTablePointer(reinterpret_cast<const multiboot_v2_info*>(info));
            break;
    }

    run();
}

EFIAPI EFI_STATUS _IgniterStage2EfiEntry(EFI_HANDLE, EFI_SYSTEM_TABLE*)
{
    return 0;
}

EFIAPI EFI_STATUS _IgniterStage2Efi32Entry(EFI_HANDLE, EFI_SYSTEM_TABLE*)
{
    return 0;
}

EFIAPI EFI_STATUS _IgniterStage2Efi64Entry(EFI_HANDLE, EFI_SYSTEM_TABLE*)
{
    return 0;
}

void run()
{
    // Configure logging to use some pre-allocated memory as a backing buffer.
    configureLogging(s_logBuffer, priority::all);

    // Some VMs/emulators (e.g. Bochs) provide an I/O port for debug logging. Use it if it's enabled.
    if (X86::inb(0xe9) == 0xe9)
    {
        Logger::registerMessageProcessor([](void*, Logger::opaque_message m)
        {
            Logger::message msg(m);

            // Output the message severity.
            X86::outb(0xe9, '[');
            for (auto b : Logger::priorityString(Logger::getMessagePriority(m)))
                X86::outb(0xe9, b);
            X86::outb(0xe9, ']');
            X86::outb(0xe9, ' ');

            // Copy the message to the debugging port.
            for (auto b : msg.msg())
                X86::outb(0xe9, static_cast<std::uint8_t>(b));
                
            // Add a terminating newline.
            X86::outb(0xe9, '\n');

            msg.release();
        }, nullptr);
    }

    // Banner message.
    log(priority::notice, 
        "Igniter v0.1.0\n"
        "Copyright (C) Fraser Gordon, all rights reserved"
    );

    // Hardware probing will probably require making BIOS calls in Virtual-8086 mode. This requires being able to handle
    // interrupts so make sure that the IDT has been set up.
    //
    // This also configures the GDT, which is needed before TLS is initialised.
    configureInterruptTable();

    // Configure TLS (required for C++ exception handling).
    auto tls_data_size = _TBSS_END - _TDATA_START;
    std::span<const std::byte> tls_data {_TDATA_START, _TBSS_END};
    std::span<std::byte> boot_thread_tls {g_initThreadTLS, tls_data_size + 2*sizeof(void*)};
    System::ABI::TLS::setInitialData(tls_data_size, alignof(void*), tls_data);
    System::ABI::TLS::setThreadDataLayout(2*sizeof(void*), sizeof(void*));
    System::ABI::TLS::configureInitialThread(boot_thread_tls);

    // Register the C++ exception handling information.
    auto eh_frame_hdr = _EH_FRAME_HDR;
    auto text_start = reinterpret_cast<std::uintptr_t>(_TEXT_START);
    auto data_start = reinterpret_cast<std::uintptr_t>(_DATA_START);
    log(priority::debug, "C++: exception handling data at {} (text={:#x} data={:#x})", static_cast<const void*>(eh_frame_hdr), text_start, data_start);
    _Unwind_XVI_RegisterEhFrameHdr(eh_frame_hdr, text_start, data_start);

    // Create a VGA console. If configured for text output, log messages will be printed to the screen.
    VGAConsole console;

    // Run an initial memory probe.
    performEarlyMemoryProbe();

    // Now that we have some memory we can use, we can create the page tables and enable paging.
    enablePaging();

    // Now that we have some memory, set up the V86 supervisor so we can begin probing the BIOS.
    prepareV86Mode();

    // With both memory and V86 mode available, we can properly probe the BIOS for memory and, from there, probe the
    // rest of the hardware that we need to known about.
    hardwareProbe();

    // We shouldn't get here.
    log(priority::emergency, "FATAL: boot loader exit without starting system");
}


// Handler for uncaught exceptions.
extern "C" [[noreturn]] void abortHandler() asm("System.Abort");
void abortHandler()
{
    // Is this a C++ exception?
    auto p = std::current_exception();
    if (p)
    {
        // Print out the type of the exception.
        auto ti = __cxxabiv1::__cxa_current_exception_type();
        auto __ptr = __cxxabiv1::__cxa_demangle(ti->name(), nullptr, nullptr, nullptr);
        log(priority::emergency, "FATAL: unhandled exception type={} ({})", __ptr, ti->name());        

        // Rethrow the exception to see if it's one we can get more information on.
        try
        {
            std::rethrow_exception(p);
        }
        catch (std::exception& e)
        {
            log(priority::emergency, "FATAL: .what() = {}", e.what());
        }
        catch (...)
        {
            // Nothing further to report.
        }
    }
    else
    {
        // Not an exception; something else caused abort()/terminate() to be called.
        log(priority::emergency, "FATAL: unhandled error (non-exception)");
    }

    log(priority::alert, "--- Start backtrace ---");
    _Unwind_Backtrace([](_Unwind_Context* context, void*) -> _Unwind_Reason_Code
    {
        auto pc = _Unwind_GetIP(context);
        auto cfa = _Unwind_GetCFA(context);
        log(priority::alert, "    <{:08x}> cfa={:08x}", pc, cfa);
        return _URC_CONTINUE_UNWIND;
    }, nullptr);
    log(priority::alert, "--- End backtrace ---");

    while (true)
    {
        asm volatile ("cli\n\thlt");
    }
}


} // namespace System::Boot::Igniter

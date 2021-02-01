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
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>
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

    using AttrReg = VGA::AttributeController::Register;
    using CRTCReg = VGA::CRTC::Register;
    using GfxReg = VGA::GraphicsController::Register;
    using SeqReg = VGA::Sequencer::Register;

    VGA::VGADevice vga;

    auto& attr = vga.attributeController();
    auto& crtc = vga.crtc();
    auto& gfx = vga.graphicsController();
    auto& seq = vga.sequencer();

    /*log(priority::debug,
        "Attr: Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x}\n"
        "      Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x}\n"
        "      Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x} Palette0={:02x}\n"
        "      Palette0={:02x} ModeControl={:02x} OverscanColour={:02x} ColourPlaneEnable={:02x}\n"
        "      HorizontalPixelPan={:02x} ColourSelect={:02x}",
        attr.readRegister(AttrReg::Palette0),
        attr.readRegister(AttrReg::Palette1),
        attr.readRegister(AttrReg::Palette2),
        attr.readRegister(AttrReg::Palette3),
        attr.readRegister(AttrReg::Palette4),
        attr.readRegister(AttrReg::Palette5),
        attr.readRegister(AttrReg::Palette6),
        attr.readRegister(AttrReg::Palette7),
        attr.readRegister(AttrReg::Palette8),
        attr.readRegister(AttrReg::Palette9),
        attr.readRegister(AttrReg::Palette10),
        attr.readRegister(AttrReg::Palette11),
        attr.readRegister(AttrReg::Palette12),
        attr.readRegister(AttrReg::Palette13),
        attr.readRegister(AttrReg::Palette14),
        attr.readRegister(AttrReg::Palette15),
        attr.readRegister(AttrReg::ModeControl),
        attr.readRegister(AttrReg::OverscanColour),
        attr.readRegister(AttrReg::ColourPlaneEnable),
        attr.readRegister(AttrReg::HorizontalPixelPan),
        attr.readRegister(AttrReg::ColourSelect)
    );

    log(priority::debug,
        "CRTC: HorizontalTotal={:02x} HorizontalDisplayEnableEnd={:02x} HorizontalBlankStart={:02x}\n"
        "      HorizontalBlankEnd={:02x} HorizontalRetraceStart={:02x} HorizontalRetraceEnd={:02x}\n"
        "      VerticalTotal={:02x} Overflow={:02x} PresetRowScan={:02x} MaximumScanLineAddress={:02x}\n"
        "      CursorStart={:02x} CursorEnd={:02x} StartAddressHigh={:02x} StartAddressLow={:02x}\n"
        "      CursorLocationHigh={:02x} CursorLocationLow={:02x} VerticalRetraceStart={:02x}\n"
        "      VerticalRetraceEnd={:02x} VerticalDisplayEnableEnd={:02x} Offset={:02x}\n"
        "      UnderlineLocation={:02x} VerticalBlankStart={:02x} VerticalBlankEnd={:02x}\n"
        "      ModeControl={:02x} LineCompare={:02x}",
        crtc.readRegister(CRTCReg::HorizontalTotal),
        crtc.readRegister(CRTCReg::HorizontalDisplayEnableEnd),
        crtc.readRegister(CRTCReg::HorizontalBlankingStart),
        crtc.readRegister(CRTCReg::HorizontalBlankingEnd),
        crtc.readRegister(CRTCReg::HorizontalRetraceStart),
        crtc.readRegister(CRTCReg::HorizontalRetraceEnd),
        crtc.readRegister(CRTCReg::VerticalTotal),
        crtc.readRegister(CRTCReg::Overflow),
        crtc.readRegister(CRTCReg::PresetRowScan),
        crtc.readRegister(CRTCReg::MaximumScanLineAddress),
        crtc.readRegister(CRTCReg::CursorStart),
        crtc.readRegister(CRTCReg::CursorEnd),
        crtc.readRegister(CRTCReg::StartAddressHigh),
        crtc.readRegister(CRTCReg::StartAddressLow),
        crtc.readRegister(CRTCReg::CursorLocationHigh),
        crtc.readRegister(CRTCReg::CursorLocationLow),
        crtc.readRegister(CRTCReg::VerticalRetraceStart),
        crtc.readRegister(CRTCReg::VerticalRetraceEnd),
        crtc.readRegister(CRTCReg::VerticalDisplayEnableEnd),
        crtc.readRegister(CRTCReg::Offset),
        crtc.readRegister(CRTCReg::UnderlineLocation),
        crtc.readRegister(CRTCReg::VerticalBlankingStart),
        crtc.readRegister(CRTCReg::VerticalBlankingEnd),
        crtc.readRegister(CRTCReg::ModeControl),
        crtc.readRegister(CRTCReg::LineCompare)
    );

    log(priority::debug,
        "Gfx: SetReset={:02x} EnableSetReset={:02x} ColourCompare={:02x} DataRotate={:02x}\n"
        "     ReadMapSelect={:02x} GraphicsMode={:02x} MiscGraphics={:02x} ColourDontCare={:02x}\n"
        "     BitMask={:02x}",
        gfx.readRegister(GfxReg::SetReset),
        gfx.readRegister(GfxReg::EnableSetReset),
        gfx.readRegister(GfxReg::ColourCompare),
        gfx.readRegister(GfxReg::DataRotate),
        gfx.readRegister(GfxReg::ReadMapSelect),
        gfx.readRegister(GfxReg::GraphicsMode),
        gfx.readRegister(GfxReg::MiscGraphics),
        gfx.readRegister(GfxReg::ColourDontCare),
        gfx.readRegister(GfxReg::BitMask)
    );

    log(priority::debug,
        "Sequencer: Reset={:02x} ClockingMode={:02x} MapMask={:02x} CharacterMap={:02x} MemoryMode={:02x}",
        seq.readRegister(SeqReg::Reset),
        seq.readRegister(SeqReg::ClockingMode),
        seq.readRegister(SeqReg::MapMask),
        seq.readRegister(SeqReg::CharacterMap),
        seq.readRegister(SeqReg::MemoryMode)
    );*/

    // Run an initial memory probe.
    performEarlyMemoryProbe();

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
        log(priority::emergency, "FATAL: unhandled exception type={}", __ptr);        

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

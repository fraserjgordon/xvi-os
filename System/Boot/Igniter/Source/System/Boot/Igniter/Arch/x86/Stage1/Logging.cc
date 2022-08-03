#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>


#include <System/HW/CPU/Arch/x86/IO/IO.hh>
#include <System/Utility/Logger/Processor.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/BIOSCall.hh>


namespace System::Boot::Igniter
{


namespace Logger = System::Utility::Logger;


struct cursor_pos
{
    std::uint8_t row = 0;
    std::uint8_t column = 0;
};


static cursor_pos vgaGetCursor()
{
    bios_call_params params;
    params.ah = 0x03;
    params.bh = 0;

    BIOSCall(0x10, &params);

    return {.row = params.dh, .column = params.dl};
}


static void vgaSetCursor(cursor_pos pos)
{
    bios_call_params params;
    params.ah = 0x02;
    params.bh = 0;
    params.dh = pos.row;
    params.dl = pos.column;

    BIOSCall(0x10, &params);
}


static void vgaWriteString(cursor_pos pos, std::string_view sv, bool update_cursor)
{
    auto linear = reinterpret_cast<std::uint32_t>(sv.data());

    bios_call_params params;
    params.ah = 0x13;
    params.al = 0;
    params.bh = 0;
    params.cx = sv.size();
    params.dh = pos.row;
    params.dl = pos.column;
    params.es = (linear / 16);
    params.bp = (linear % 16);

    if (update_cursor)
        params.al |= 0x01;

    BIOSCall(0x10, &params);
}


static void bochsWriteChar(char c)
{
    HW::CPU::X86::outb(0x00E9, static_cast<std::uint8_t>(c));
}


static void bochsWriteString(std::string_view sv)
{
    HW::CPU::X86::outsb(0x00E9, std::span{reinterpret_cast<const std::uint8_t*>(sv.data()), sv.size()});
}


void configureLogging()
{
    Logger::registerMessageProcessor([](void*, Logger::opaque_message m)
    {
        Logger::message msg(m);

        bochsWriteChar('[');
        bochsWriteString(Logger::priorityString(msg.priority()));
        bochsWriteChar(']');
        bochsWriteChar(' ');
        bochsWriteString(msg.msg());
        bochsWriteChar('\n');

        msg.release();
    }, nullptr);

    Logger::setMinimumPriority(priority::trace);
    bochsWriteChar('\n');
}


} // namespace System::Boot::Igniter

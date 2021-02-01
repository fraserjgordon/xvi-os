#include <System/HW/CPU/Arch/x86/VM86/Emulator.hh>

#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/Utility/Optional.hh>

#include <System/HW/CPU/Arch/x86/IO/IO.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>


namespace System::HW::CPU::X86
{


bool VM86Emulator::generalProtectionException(std::uint32_t error_code, vm86_interrupt_frame_t& frame, general_regs32_t& regs)
{
    // All of the opcodes requiring emulation generate an error code of zero. A non-zero error code suggests something
    // else has happened.
    if (error_code != 0)
        return false;

    // Decode the faulting instruction.
    realmode_ptr<Opcode>        ip{frame.cs, frame.ip};
    realmode_ptr<Opcode>        original_ip = ip;
    decoded_insn insn;
    bool explicit_segment = false;
    bool more = false;

    insn.ip = ip;

    // Utility function for reading an immediate byte.
    auto readImmediate = [&insn, &ip]()
    {
        auto ptr = realmode_ptr_cast<const std::uint8_t>(++ip);
        
        insn.immediate = static_cast<std::uint8_t>(*ptr);
        insn.length++;
        return insn.immediate;
    };

    do
    {
        // Assume (until we confirm otherwise) that we've reached the actual opcode part of the instruction.
        more = false;
        insn.op = *ip;
        insn.length++;
        
        switch (insn.op)
        {
            case Opcode::O32:
                insn.o32 = true;
                more = true;
                break;

            case Opcode::A32:
                insn.a32 = true;
                more = true;
                break;

            case Opcode::CS:
            case Opcode::DS:
            case Opcode::ES:
            case Opcode::FS:
            case Opcode::GS:
            case Opcode::SS:
                insn.segment = insn.op;
                explicit_segment = true;
                more = true;
                break;

            case Opcode::Lock:
                insn.lock = true;
                more = true;
                break;

            case Opcode::REP:
                insn.repeat = true;
                more = true;
                break;

            case Opcode::REPNE:
                // Not valid with any of the emulated instructions.
                return false;

            case Opcode::CLI:
            case Opcode::STI:
            case Opcode::PUSHF:
            case Opcode::POPF:
            case Opcode::IRET:
                checkAndEmulateInterruptFlagChange(insn, frame, regs);
                break;

            case Opcode::INT:
            {
                auto vector = readImmediate();
                dispatchSoftwareInterrupt(insn, frame, regs, vector);
                break;
            }

            case Opcode::INB:
            {
                auto port = regs.dx;
                checkAndEmulateIO(insn, frame, regs, {port, IO::In, OperationSize::Byte, 0});
                break;
            }

            case Opcode::INBi8:
            {
                std::uint16_t port = readImmediate();
                checkAndEmulateIO(insn, frame, regs, {port, IO::In, OperationSize::Byte, 0});
                break;
            }

            case Opcode::INW:
            {
                auto port = regs.dx;
                checkAndEmulateIO(insn, frame, regs, {port, IO::In, insn.o32 ? OperationSize::Long : OperationSize::Word, 0});
                break;
            }

            case Opcode::INWi8:
            {
                std::uint16_t port = readImmediate();
                checkAndEmulateIO(insn, frame, regs, {port, IO::In, insn.o32 ? OperationSize::Long : OperationSize::Word, 0});
                break;
            }

            case Opcode::OUTB:
            {
                auto port = regs.dx;
                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, OperationSize::Byte, 0});
                break;
            }

            case Opcode::OUTBi8:
            {
                std::uint16_t port = readImmediate();
                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, OperationSize::Byte, 0});
                break;
            }

            case Opcode::OUTW:
            {
                auto port = regs.dx;
                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, insn.o32 ? OperationSize::Long : OperationSize::Word, 0});
                break;
            }

            case Opcode::OUTWi8:
            {
                std::uint16_t port = readImmediate();
                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, insn.o32 ? OperationSize::Long : OperationSize::Word, 0});
                break;
            }

            case Opcode::INSB:
            {
                auto port = regs.dx;
                realmode_ptr<void> di{frame.es, regs.di};
                std::uint32_t count = 1;

                if (!explicit_segment)
                    insn.segment = Opcode::ES;

                if (insn.repeat)
                    count = insn.o32 ? regs.ecx : regs.cx;

                checkAndEmulateIO(insn, frame, regs, {port, IO::In, OperationSize::Byte, count, di});
                break;
            }

            case Opcode::INSW:
            {
                auto port = regs.dx;
                realmode_ptr<void> di{frame.es, regs.di};
                std::uint32_t count = 1;

                if (!explicit_segment)
                    insn.segment = Opcode::ES;

                if (insn.repeat)
                    count = insn.o32 ? regs.ecx : regs.cx;

                checkAndEmulateIO(insn, frame, regs, {port, IO::In, insn.o32 ? OperationSize::Long : OperationSize::Word, count, di});
                break;
            }

            case Opcode::OUTSB:
            {
                auto port = regs.dx;
                realmode_ptr<void> si{getSegment(insn.segment, frame), regs.si};
                std::uint32_t count = 1;

                if (insn.repeat)
                    count = insn.o32 ? regs.ecx : regs.cx;

                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, OperationSize::Byte, count, si});
                break;
            }

            case Opcode::OUTSW:
            {
                auto port = regs.dx;
                realmode_ptr<void> si{getSegment(insn.segment, frame), regs.si};
                std::uint32_t count = 1;

                if (insn.repeat)
                    count = insn.o32 ? regs.ecx : regs.cx;

                checkAndEmulateIO(insn, frame, regs, {port, IO::Out, insn.o32 ? OperationSize::Long : OperationSize::Word, count, si});
                break;
            }

            default:
                // #GP exception was caused by something else.
                return false;
        }

        ++ip;
    }
    while (more);

    // Write back the updated instruction pointer (if emulation didn't cause it to be modified as a side-effect).
    if (original_ip.segment() == frame.cs && original_ip.offset() == frame.ip)
    {
        // Note: the segment will never change as a result of incrementing %ip.
        frame.ip = ip.offset();
    }

    // Operation was emulated successfully.
    return true;
}


bool VM86Emulator::checkAndRaiseSegmentOverrun(vm86_interrupt_frame_t& frame, Opcode segment, std::uint32_t offset, std::size_t access_size)
{
    // Segment overruns are any accesses beyond the normal 16-bit segment limit.
    auto valid = [](std::uint32_t ptr, std::size_t size)
    {
        auto limit = std::numeric_limits<std::uint16_t>::max();
        if (ptr > limit || (ptr + size))
            return false;
        return true;
    };

    if (valid(offset, access_size))
        return false;

    // We raise a #SS exception if we're using the %ss segment or a #GP for any other segment.
    auto vector = (segment == Opcode::SS) ? SS : GP;
    injectInterrupt(frame, vector);

    // We raised an exception so no further processing should be carried out.
    return true;
}


bool VM86Emulator::checkInterruptFlagChange(const decoded_insn& insn, vm86_interrupt_frame_t& frame, general_regs32_t& regs)
{
    // Call the hook (if it exists) for any policy for the requested I/O operation.
    auto result = DefaultInterruptFlagPolicy;
    if (m_interruptFlagHook)
        result = m_interruptFlagHook(insn, frame, regs);

    // Take any error action indicated by the policy.
    switch (result)
    {
        case PolicyResult::Emulate:
            // The caller needs to emulate the I/O.
            break;

        case PolicyResult::Fatal:
        default:
            // Fatal emulation error.
            fatalError(frame, regs);
            return false;

        case PolicyResult::Fault:
            // Inject a #GP fault.
            injectInterrupt(frame, GP);
            return false;

        case PolicyResult::Ignore:
            // The hook performed any necessary actions.
            return false;
    }

    return true;
}


bool VM86Emulator::checkAndEmulateInterruptFlagChange(const decoded_insn& insn, vm86_interrupt_frame_t& frame, general_regs32_t& regs)
{
    // Check whether we need to emulate the interrupt flag change.
    if (!checkInterruptFlagChange(insn, frame, regs))
        return true;

    // Emulate the instruction.
    realmode_ptr<std::uint16_t> sp{frame.ss, frame.sp};
    switch (insn.op)
    {
        case Opcode::CLI:
            // Clear the interrupt flag (disable interrupts).
            frame.eflags.bits._if = 0;
            return true;

        case Opcode::STI:
            // Set the interrupt flag (enable interrupts).
            frame.eflags.bits._if = 1;
            return true;

        case Opcode::PUSHF:
            // Store the processor flags to the stack. We don't hide any bits from the VM86 code.
            if (insn.o32)
                *realmode_ptr_cast<std::uint32_t>(sp -= 2) = frame.eflags.uint32;
            else
                *--sp = static_cast<std::uint16_t>(frame.eflags.uint32);
            frame.sp = sp.offset();
            return true;

        case Opcode::POPF:
            // Restore the processor flags from the stack. The VM bit needs to be forced on, however.
            if (insn.o32)
            {
                auto raw = *realmode_ptr_cast<std::uint32_t>(sp);
                sp += 2;
                frame.eflags.uint32 = raw;
                frame.eflags.bits.vm = 1;
            }
            else
            {
                auto raw = *sp++;
                frame.eflags.uint32 = (frame.eflags.uint32 & 0xFFFF0000) | raw;
            }
            frame.sp = sp.offset();
            return true;

        case Opcode::IRET:
            // Perform a return from interrupt.
            if (insn.o32)
            {
                // The 32-bit 'iret' in real-mode does complex merging of %eflags values. The masks used here are taken
                // from the Intel system instruction manual.
                //
                // Note that all 32 bits of %eip are popped here.
                auto esp = realmode_ptr_cast<std::uint32_t>(sp);
                frame.eip = *esp++;
                frame.cs = static_cast<std::uint16_t>(*esp++);
                auto flags = *esp++;
                frame.eflags.uint32 = (flags & 0x00257FD5) | (frame.eflags.uint32 & 0x001A0000);
                frame.sp = esp.offset();
            }
            else
            {
                frame.ip = *sp++;
                frame.cs = *sp++;
                frame.eflags.uint32 = (frame.eflags.uint32 & 0xFFFF0000) | *sp++;
                frame.sp = sp.offset();
            }
            return true;

        default:
            // Invalid opcode for this operation...
            return false;
    }
}


bool VM86Emulator::checkIO(const decoded_insn& insn, vm86_interrupt_frame_t& frame, general_regs32_t& regs, const io_params& io)
{
    // Call the hook (if it exists) for any policy for the requested I/O operation.
    auto result = DefaultIoPolicy;
    if (m_ioHook)
        result = m_ioHook(insn, frame, regs, io);

    // Take any error action indicated by the policy.
    switch (result)
    {
        case PolicyResult::Emulate:
            // The caller needs to emulate the I/O.
            break;

        case PolicyResult::Fatal:
        default:
            // Fatal emulation error.
            fatalError(frame, regs);
            return false;

        case PolicyResult::Fault:
            // Inject a #GP fault.
            injectInterrupt(frame, GP);
            return false;

        case PolicyResult::Ignore:
            // The hook performed any necessary actions.
            return false;
    }

    return true;
}


bool VM86Emulator::checkAndEmulateIO(const decoded_insn& insn, vm86_interrupt_frame_t& frame, general_regs32_t& regs, const io_params& io)
{   
    // Convert all string I/O operations into repeated 1-count operations.
    if (io.string_ptr && io.count > 1)
    {
        // Same I/O parameters except the operation count.
        auto one_io = io;
        one_io.count = 1;

        for (std::uint32_t i = 0; i < io.count; ++i)
        {
            // Perform one I/O operation.
            if (!checkAndEmulateIO(insn, frame, regs, one_io))
                return false;

            // Calculate the update size.
            std::int32_t increment = static_cast<std::int32_t>(io.size);
            if (frame.eflags.bits.df)
                increment = -increment;

            // Update the string pointer.
            one_io.string_ptr = realmode_ptr_cast<void>(realmode_ptr_cast<std::byte>(*one_io.string_ptr) + increment); 

            // Update the output registers.
            switch (io.direction)
            {
                case IO::In:
                    if (insn.a32)
                        regs.edi += static_cast<std::uint32_t>(increment);
                    else
                        regs.di = static_cast<std::uint16_t>(regs.di + increment);
                    break;

                case IO::Out:
                    if (insn.a32)
                        regs.esi += static_cast<std::uint32_t>(increment);
                    else
                        regs.si = static_cast<std::uint16_t>(regs.si + increment);
                    break;
            }
        }

        return true;
    }

    // If this is a string operation, validate the output pointer. It can only cause a segment overrun if the 32-bit
    // address size prefix was used with a string I/O operation.
    if (insn.a32 && io.string_ptr)
    {
        std::uint32_t offset = (io.direction == IO::In) ? regs.edi : regs.esi;
        if (checkAndRaiseSegmentOverrun(frame, insn.segment, offset, static_cast<std::size_t>(io.size)))
            return false;
    }
    
    // Only emulate the I/O operation if we need to.
    if (!checkIO(insn, frame, regs, io))
        return true;

    // Handle simple I/O to-or-from-register cases first.
    if (!io.string_ptr)
    {
        switch (ioOperation(io.direction, io.size))
        {
            case IOOperation::InByte:
                regs.al = inb(io.port);
                return true;

            case IOOperation::InWord:
                regs.ax = inw(io.port);
                return true;

            case IOOperation::InLong:
                regs.eax = inl(io.port);
                return true;

            case IOOperation::OutByte:
                outb(io.port, regs.al);
                return true;

            case IOOperation::OutWord:
                outw(io.port, regs.ax);
                return true;

            case IOOperation::OutLong:
                outl(io.port, regs.eax);
                return true;
        }
    }

    // This must be a string operation (but we've guaranteed we're only looking at it in single-access chunks).
    switch (ioOperation(io.direction, io.size))
    {
        case IOOperation::InByte:
            *realmode_ptr_cast<std::uint8_t>(*io.string_ptr) = inb(io.port);
            return true;

        case IOOperation::InWord:
            *realmode_ptr_cast<std::uint16_t>(*io.string_ptr) = inw(io.port);
            return true;

        case IOOperation::InLong:
            *realmode_ptr_cast<std::uint32_t>(*io.string_ptr) = inl(io.port);
            return true;

        case IOOperation::OutByte:
            outb(io.port, *realmode_ptr_cast<std::uint8_t>(*io.string_ptr));
            return true;

        case IOOperation::OutWord:
            outw(io.port, *realmode_ptr_cast<std::uint16_t>(*io.string_ptr));
            return true;

        case IOOperation::OutLong:
            outl(io.port, *realmode_ptr_cast<std::uint32_t>(*io.string_ptr));
            return true;
    }

    // Shouldn't get here...
    return false;
}


bool VM86Emulator::injectInterrupt(vm86_interrupt_frame_t& frame, std::uint8_t vector)
{
    // Stack pointer.
    //
    // The truncation of %esp to %sp here matches the behaviour of real hardware.
    realmode_ptr<std::uint16_t> sp{frame.ss, static_cast<std::uint16_t>(frame.esp)};
    
    // Generate the exception frame on the real-mode stack. Note that this might underflow the stack and corrupt random
    // memory (within the %ss segment); it's unclear what a real i8086 would do in this situation.
    *--sp = static_cast<std::uint16_t>(frame.eflags.uint32);
    *--sp = frame.cs;
    *--sp = static_cast<std::uint16_t>(frame.eip);

    // Update the real frame with the updated stack pointer and exception destination.
    auto destination = m_ivt->getDestination(vector);
    frame.esp = sp.offset();
    frame.cs = destination.segment();
    frame.eip = destination.offset();

    return true;
}


void VM86Emulator::dispatchSoftwareInterrupt(const decoded_insn& insn, vm86_interrupt_frame_t& frame, general_regs32_t& regs, std::uint8_t vector)
{
    // Call the hook (if it exists) for any policy for the requested I/O operation.
    auto result = DefaultSoftwareInterruptPolicy;
    if (m_softwareInterruptHook)
        result = m_softwareInterruptHook(insn, frame, regs, vector);

    // Take any error action indicated by the policy.
    switch (result)
    {
        case PolicyResult::Emulate:
            // The caller needs to emulate the I/O. Update the frame instruction pointer to point after the 'int' opcode
            // then inject the interrupt.
            frame.ip = static_cast<std::uint16_t>(insn.ip.offset() + insn.length);
            injectInterrupt(frame, vector);
            break;

        case PolicyResult::Fatal:
        default:
            // Fatal emulation error.
            fatalError(frame, regs);
            break;

        case PolicyResult::Fault:
            // Inject a #GP fault.
            injectInterrupt(frame, GP);
            break;

        case PolicyResult::Ignore:
            // The hook performed any necessary actions.
            break;
    }
}


void VM86Emulator::fatalError(vm86_interrupt_frame_t& frame, general_regs32_t& regs)
{
    if (m_fatalErrorHook)
        m_fatalErrorHook(frame, regs);
}


std::uint16_t VM86Emulator::getSegment(std::optional<Opcode> prefix, const vm86_interrupt_frame_t& frame)
{
    // All of the instructions with overridable segments are the string I/O instructions. With those, only the
    // outsb/outsw/outsl instructions can have the segment changed and those instructions default to using %ds if not
    // otherwise specified.
    if (!prefix)
        prefix = Opcode::DS;

    switch (*prefix)
    {
        case Opcode::DS:
            return frame.ds;

        case Opcode::ES:
            return frame.es;

        case Opcode::FS:
            return frame.fs;

        case Opcode::GS:
            return frame.gs;

        case Opcode::SS:
            return frame.ss;

        case Opcode::CS:
            return frame.cs;

        default:
            // Shouldn't happen but we need to return something...
            return frame.ds;
    }
}


} // namespace System::HW::CPU::X86

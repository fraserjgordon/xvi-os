#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_EMULATOR_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_EMULATOR_H


#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Function.hh>
#include <System/C++/Utility/Optional.hh>

#include <System/HW/CPU/Arch/x86/Segmentation/IVT.hh>

#include <System/HW/CPU/Arch/x86/VM86/Context.hh>


namespace System::HW::CPU::X86
{

class VM86Emulator
{
public:

    // Opcodes (and opcode prefixes) that may need to be emulated.
    enum class Opcode : std::uint8_t
    {
        // Instruction prefixes.
        O32     = 0x66,     // Operand size override.
        A32     = 0x67,     // Address size override.
        CS      = 0x2E,     // Use %cs segment.
        DS      = 0x3E,     // Use %ds segment.
        ES      = 0x26,     // Use %es segment.
        FS      = 0x64,     // Use %fs segment.
        GS      = 0x65,     // Use %gs segment.
        SS      = 0x36,     // Use %ss segment.
        Lock    = 0xF0,     // Use locked memory access.
        REP     = 0xF3,     // Repeat opcode based on %cx / conditional check.
        REPNE   = 0xF2,     // Repeat opcode based on %cx / conditional check.

        // Instructions sensitive to IOPL.
        CLI     = 0xFA,     // Clear interrupt flag.
        STI     = 0xFB,     // Set interrupt flag.
        PUSHF   = 0x9C,     // Push flags register.
        POPF    = 0x9D,     // Pop flags register.
        INT     = 0xCD,     // Software interrupt.
        IRET    = 0xCF,     // Return from interrupt.
        INB     = 0xEC,     // Read byte from I/O port.
        INBi8   = 0xE4,     // Read byte from I/O port (8-bit immediate).
        INW     = 0xED,     // Read byte from I/O port.
        INWi8   = 0xE5,     // Read word from I/O port (8-bit immediate).
        OUTB    = 0xEE,     // Write byte to I/O port.
        OUTBi8  = 0xE6,     // Write byte to I/O port (8-bit immediate).
        OUTW    = 0xEF,     // Write word to I/O port.
        OUTWi8  = 0xE7,     // Write word to I/O port (8-bit immediate).
        INSB    = 0x6C,     // Read byte from I/O port to %es:%di and increment.
        INSW    = 0x6D,     // Read word from I/O port to %es:%di and increment.
        OUTSB   = 0x6E,     // Write byte to I/O port from %ds:%si and increment.
        OUTSW   = 0x6F,     // Write word to I/O port from %ds:%si and increment.
    };

    // Operation sizes.
    enum class OperationSize
    {
        Byte    = 1,
        Word    = 2,
        Long    = 4,
    };

    // I/O directions.
    enum class IO
    {
        In      = 0,
        Out     = 8,
    };

    // Combination of the IO and OperationSize enums.
    enum class IOOperation
    {
        InByte  = 1,
        InWord  = 2,
        InLong  = 4,
        OutByte = 9,
        OutWord = 10,
        OutLong = 12,
    };

    // Policy decisions that hooks can return.
    enum class PolicyResult
    {
        Ignore,             // Hook has handled the action internally; no further action required.
        Emulate,            // The emulator should perform the requested action.
        Fault,              // The emulator should inject a #GP (or other appropriate exception).
        Fatal,              // The emulator should treat this as a fatal error.
    };

    // Decoded instruction information.
    struct decoded_insn
    {
        realmode_ptr<Opcode>    ip{0, 0};               // Location of the instruction.
        std::uint8_t            length = 0;             // Instruction length (<= 15 bytes).
        Opcode                  op;                     // Opcode (without prefixes or suffixes).
        bool                    a32 = false;            // Set if 32-bit addressing is to be used.
        bool                    o32 = false;            // Set if 32-bit operand size is to be used.
        bool                    repeat = false;         // Set if the "rep" prefix was used.
        bool                    lock = false;           // Set if the "lock" prefix was used.
        Opcode                  segment = Opcode::DS;   // Selected segment (explicit or implied).
        std::uint8_t            immediate = 0;          // Any constant encoded directly in the instruction.
    };

    // I/O operation parameters.
    struct io_params
    {
        std::uint16_t   port;
        IO              direction;
        OperationSize   size;
        std::uint32_t   count;
        std::optional<realmode_ptr<void>> string_ptr = std::nullopt;
    };

    // Signatures for various policy hooks.
    using software_interrupt_hook_t = std::function<PolicyResult(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&, std::uint8_t)>;
    using fatal_error_hook_t = std::function<void(vm86_interrupt_frame_t&, general_regs32_t&)>;
    using interrupt_flag_hook_t = std::function<PolicyResult(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&)>;
    using io_hook_t = std::function<PolicyResult(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&, const io_params&)>;


    // Default policies for the hooks.
    static constexpr auto DefaultSoftwareInterruptPolicy = PolicyResult::Emulate;
    static constexpr auto DefaultInterruptFlagPolicy = PolicyResult::Fatal;
    static constexpr auto DefaultIoPolicy = PolicyResult::Fatal;

    // Vectors for particular hardware errors.
    static constexpr std::uint8_t SS    = 12;   // Stack fault.
    static constexpr std::uint8_t GP    = 13;   // General protection fault.

    // Recommended instruction sequence to use for trapping to the emulator from the VM86 code.
    //
    // Any sequence beginning c4 c4 ... is invalid in VM86 mode (but may represent a valid VEX prefix in protected
    // mode!). 32-bit versions of Windows use this sequence to make calls to the NTVDM API so that means:
    //  - it's unlikely to be found in real-mode code anywhere else and won't cause unintended breaks
    //  - Intel/AMD are aware of this usage and won't gratuitously break it.
    static constexpr std::initializer_list<std::byte> EmulatorBreak = { std::byte{0xC4}, std::byte{0xC4}, std::byte{0x00} };


    bool overflowException();
    bool boundsException();

    // Determines if the cause of the #GP exception is an IOPL-sensitive or other privileged instruction and, if so,
    // handles it.
    //
    // Returns true if the exception is consumed by the VM86 monitor and false if it still needs to be handled.
    bool generalProtectionException(vm86_interrupt_frame_t& frame, general_regs32_t& regs);


    static constexpr IOOperation ioOperation(IO direction, OperationSize size)
    {
        return static_cast<IOOperation>(static_cast<int>(direction) | static_cast<int>(size));
    }


private:

    // Pointer to the interrupt vector table to be used while emulating interrupts.
    const InterruptVectorTable* m_ivt = nullptr;

    // Hooks for implementing policy decisions.
    software_interrupt_hook_t   m_softwareInterruptHook = {};
    fatal_error_hook_t          m_fatalErrorHook = {};
    interrupt_flag_hook_t       m_interruptFlagHook = {};
    io_hook_t                   m_ioHook = {};


    // Checks that the given offset is valid (i.e doesn't overrun a segment). If it does overrun, emulates the exception
    // that would have been generated in real-mode.
    bool checkAndRaiseSegmentOverrun(vm86_interrupt_frame_t&, Opcode segment, std::uint32_t offset, std::size_t access_size);

    // Checks that changing the interrupt flag is permitted.
    bool checkInterruptFlagChange(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&);

    // Calls checkInterruptFlagChange then emulates if that was the desired outcome.
    //
    // Returns true on success or false if any errors were encountered (including #GP fault injection).
    bool checkAndEmulateInterruptFlagChange(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&);

    // Checks whether access to the given I/O port should be performed by this emulator.
    bool checkIO(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&, const io_params&);

    // Calls checkIO then emulates if that was the desired outcome.
    //
    // Returns true on success or false if any errors were encountered (including #GP fault injection).
    bool checkAndEmulateIO(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&, const io_params&);

    // Injects the given interrupt into the emulated program.
    bool injectInterrupt(vm86_interrupt_frame_t&, std::uint8_t vector);

    // Dispatches the given software interrupt.
    void dispatchSoftwareInterrupt(const decoded_insn&, vm86_interrupt_frame_t&, general_regs32_t&, std::uint8_t vector);

    // A fatal emulation error has occurred.
    void fatalError(vm86_interrupt_frame_t&, general_regs32_t&);


    static std::uint16_t getSegment(std::optional<Opcode> prefix, const vm86_interrupt_frame_t& frame);
};

} // namespace System::HW:CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_EMULATOR_H */

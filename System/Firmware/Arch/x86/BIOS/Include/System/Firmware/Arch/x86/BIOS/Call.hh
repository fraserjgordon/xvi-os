#pragma once
#ifndef __SYSTEM_FIRMWARE_ARCH_X86_BIOS_CALL_H
#define __SYSTEM_FIRMWARE_ARCH_X86_BIOS_CALL_H


#include <cstdint>
#include <functional>
#include <span>

#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>


namespace System::Firmware::X86::BIOS
{


// Parameters that may be passed to a BIOS call. Not all registers are available; the %esp register cannot be given nor
// can the %cs and %ss segment registers.
struct call_params_t
{
    // General-purpose registers.
    union { std::uint32_t eax = 0; std::uint16_t ax; struct { std::uint8_t al, ah; }; };
    union { std::uint32_t ebx = 0; std::uint16_t bx; struct { std::uint8_t bl, bh; }; };
    union { std::uint32_t ecx = 0; std::uint16_t cx; struct { std::uint8_t cl, ch; }; };
    union { std::uint32_t edx = 0; std::uint16_t dx; struct { std::uint8_t dl, dh; }; };
    union { std::uint32_t esi = 0; std::uint16_t si; struct { std::uint8_t sil, sih; }; };
    union { std::uint32_t edi = 0; std::uint16_t di; struct { std::uint8_t dil, dih; }; };
    union { std::uint32_t ebp = 0; std::uint16_t bp; struct { std::uint8_t bpl, bph; }; };

    // Segment registers.
    std::uint16_t ds = 0;
    std::uint16_t es = 0;
    std::uint16_t fs = 0;
    std::uint16_t gs = 0;
};


// Performs a call into the BIOS.
//
// The return value is the inverse of the carry flag (usually used by the BIOS to indicate whether the requested service
// is supported; a clear carry flag means it is while a set flag indicates no support).
bool call(const call_params_t& parameters, call_params_t& results);

// Performs a call into the BIOS with an input buffer via the %ds:%si registers.
bool callWithInputRaw(const call_params_t& parameters, const void* buffer, std::size_t length, call_params_t& results);

// Wrapper for callWithInputRaw.
template <class T>
bool callWithInput(const call_params_t& parameters, std::span<T> buffer, call_params_t& results)
{
    return callWithInputRaw(params, buffer.data(), buffer.size_bytes(), results);
}

// Performs a call into the BIOS that fills an output buffer via the %es:%di registers.
bool callWithOutputRaw(const call_params_t& parameters, call_params_t& results, void* buffer, std::size_t length);

// Wrapper for callWithOutputRaw.
template <class T>
bool callWithOutput(const call_params_t& parameters, call_params_t& results, std::span<T> buffer)
{
    return callWithOutput(parameters, results, buffer.data(), buffer.size_bytes());
}


class CallEnvironment
{
public:

    using realmode_ptr = System::HW::CPU::X86::realmode_ptr<void>;

    // Allocates a bounce buffer in the first 1MiB of address space. This buffer is only valid for the duration of the
    // call (it is automatically freed immediately after the post-hook completes).
    //
    // Note that the space for buffers may be shared with the stack and may be very limited in space.
    realmode_ptr allocateBuffer(std::uint16_t size);
};

using call_pre_hook_t = std::function<bool(call_params_t&, CallEnvironment&)>;
using call_post_hook_t = std::function<void(call_params_t&)>;

bool callWithHooks(const call_params_t& parameters, call_pre_hook_t pre_hook, call_post_hook_t post_hook, call_params_t& results);


} // namespace System::Firmware::X86::BIOS


#endif /* ifndef __SYSTEM_FIRMWARE_ARCH_X86_BIOS_CALL_H */

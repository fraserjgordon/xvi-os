#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint32_t __udivmodsi4(__uint32_t n, __uint32_t d, __uint32_t* r)
{
    auto result = System::ABI::Integer::udiv(n, d);
    *r = result.r;
    return result.q;
}

/*#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
asm
(R"(

.global __aeabi_uidivmod
.protected __aeabi_uidivmod
.type __aeabi_uidivmod, "function"
.size __aeabi_uidivmod, __aeabi_uidivmod_end - __aeabi_uidivmod

__aeabi_uidivmod:

    sub     sp, #4
    mov     r2, sp
    bl      __aeabi_uidivmod_impl
    ldr     r1, [sp]
    add     sp, #4
    mov     pc, lr

__aeabi_uidivmod_end = .

)");
#endif*/

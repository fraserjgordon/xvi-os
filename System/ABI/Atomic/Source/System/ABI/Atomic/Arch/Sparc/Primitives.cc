#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


namespace System::ABI::Atomic
{


void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    __uint8_t result = 0xff;
    while (result != 0)
    {
        asm volatile
        (R"(
            ldstub  [ %[lock] ], %[result]
        )"
            : [result] "=r" (result)
            : [lock] "r" (lock)
            : "memory"
        );
    }
}

void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    *lock = 0;
}

}

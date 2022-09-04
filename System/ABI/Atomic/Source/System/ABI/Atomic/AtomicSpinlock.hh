#pragma once
#ifndef __SYSTEM_ABI_ATOMIC_ATOMICSPINLOCK_H
#define __SYSTEM_ABI_ATOMIC_ATOMICSPINLOCK_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Atomic
{

class AtomicSpinlockArray
{
public:

    using spinlock_t = std::uintptr_t;
    static constexpr std::size_t NumLocks = 1024;

    // Returns a pointer to the lock to use for the given object.
    volatile spinlock_t* getLock(const volatile void* ptr)
    {
        // Fold the pointer into an index.
        if constexpr (sizeof(__uintptr_t) == sizeof(__uint64_t))
        {
            // Drop the bottom four bits (leaving 60) which we then fold together.
            auto uintptr = __uintptr_t(ptr);
            auto index = (uintptr >> 4);
            index >>= 4;
            index ^= (index >> 30);
            index ^= (index >> 10) ^ (index >> 20);
            index %= NumLocks;
            return &m_locks[index];
        }
        else
        {
            // Drop the bottom two bits (leaving 30) which we then fold together.
            auto uintptr = __uintptr_t(ptr);
            auto index = ((uintptr >> 2) ^ (uintptr >> 12) ^ (uintptr >> 22)) % NumLocks;
            return &m_locks[index];
        }
    }

private:

    // The array of locks.
    //
    // Ideally, these would all be located in different cache lines...
    volatile spinlock_t m_locks[NumLocks] = {};
};


// The set of global locks used for objects that don't have native atomic support.
inline AtomicSpinlockArray g_atomicSpinlocks;


// Locking and unlocking methods.
inline void __Atomic_lock_acquire(volatile AtomicSpinlockArray::spinlock_t*);
inline void __Atomic_lock_release(volatile AtomicSpinlockArray::spinlock_t*);


} // namespace System::ABI::Atomic


#endif /* ifndef __SYSTEM_ABI_ATOMIC_ATOMICSPINLOCK_H */

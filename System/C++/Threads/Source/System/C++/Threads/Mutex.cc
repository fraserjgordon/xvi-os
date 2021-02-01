#include <System/C++/Threads/Mutex.hh>

#include <System/ABI/C++/Exception.hh>
#include <System/C++/Threads/MemoryWait.hh>


namespace __XVI_STD_THREADS_NS
{


inline constexpr uintptr_t MutexUnlocked    = 0;
inline constexpr uintptr_t MutexLockedFlag  = 0b01;
inline constexpr uintptr_t MutexWaitersFlag = 0b10;


void mutex::mutexLock(uintptr_t& memory)
{
    while (true)
    {
        // First, attempt the fast path: lock the mutex when free.
        atomic_ref sync_word(memory);
        uintptr_t expect = MutexUnlocked;
        if (sync_word.compare_exchange_strong(expect, MutexLockedFlag)) [[likely]]
            return;

        // Fast path failed: the lock is already held. Set the "has waiters" flag.
        if (!(expect & MutexWaitersFlag) && !sync_word.compare_exchange_weak(expect, expect|MutexWaitersFlag, memory_order::relaxed))
        {
            // Failed to set the flag - sync word might have been modified. Try again from the beginning.
            continue;
        }

        // Now that the flag is set, any unlock attempts will cause any waiters on this address to be notified. Sleep
        // until such an event.
        //
        // When this call returns, restart the loop and try to acquire the lock.
        waitOnAddress(&memory, &expect, sizeof(uintptr_t));
    }
}

bool mutex::mutexTryLock(uintptr_t& memory)
{
    // Attempt to acquire the lock.
    atomic_ref sync_word(memory);
    uintptr_t expect = MutexUnlocked;
    return sync_word.compare_exchange_strong(expect, MutexLockedFlag);
}

void mutex::mutexUnlock(uintptr_t& memory)
{
    // Atomically unlock the mutex.
    atomic_ref sync_word(memory);
    auto old_value = sync_word.exchange(MutexUnlocked);

    // If the "has waiters" flag is set, notify the waiters of the unlock.
    if (old_value & MutexWaitersFlag)
        notifyAddress(&memory, false);
}

void mutex::mutexDestroy(uintptr_t& memory)
{
    // If the mutex is held, terminate the program.
    atomic_ref sync_word(memory);
    if (sync_word.load() & MutexLockedFlag)
        System::ABI::CXX::terminate();
}


}

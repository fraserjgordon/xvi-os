#include <System/Concurrency/Seqlock.hh>

#include <atomic>

#include <System/Concurrency/SpinlockHint.hh>


namespace System::Concurrency
{


seqlock_cookie_t SeqlockReadAcquire(seqlock_t* lock)
{
    return {std::atomic_ref(lock->opaque).load(std::memory_order::acquire)};
}


bool SeqlockReadValid(seqlock_t* lock, seqlock_cookie_t cookie)
{
    std::atomic_thread_fence(std::memory_order::release);
    if ((cookie.opaque & 1) || std::atomic_ref(lock->opaque).load(std::memory_order::relaxed) != cookie.opaque)
        [[unlikely]] return false;

    return true;
}


void SeqlockWriteAcquire(seqlock_t* lock)
{
    // Write locks are obtained by incrementing the lock from an even to an odd integer.
    std::atomic_ref lockword(lock->opaque);
    std::uintptr_t current = 0;
    std::uintptr_t desired = 1;
    while (!lockword.compare_exchange_weak(current, desired, std::memory_order::acquire, std::memory_order::relaxed))
    {
        [[unlikely]];

        if (current & 1)
        {
            // Something else is currently writing - spin until they're done.
            while ((current = lockword.load(std::memory_order_relaxed)) & 1)
            {
                [[unlikely]];
                SPINLOCK_YIELD_HINT;
            }
        }

        desired = current + 1;
    }
}


void SeqlockWriteRelease(seqlock_t* lock)
{
    // Write locks are released by incrementing the lock from an odd to an even integer.
    std::atomic_ref lockword(lock->opaque);
    lockword++;
}


} // namespace System::Concurrency

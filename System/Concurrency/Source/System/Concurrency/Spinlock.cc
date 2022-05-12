#include <System/Concurrency/Spinlock.hh>

#include <atomic>

#include <System/Concurrency/SpinlockHint.hh>


namespace System::Concurrency
{


constexpr std::uintptr_t QueuedSpinlockFlags    = 0x03;
constexpr std::uintptr_t QueuedSpinlockPointer  = ~QueuedSpinlockFlags;

constexpr std::uintptr_t QueuedSpinlockLocked = 0x01;


static inline bool IsLocked(std::uintptr_t spinlock)
{
    return spinlock & QueuedSpinlockLocked;
}

static inline queued_spinlock_token_t* GetNext(std::uintptr_t spinlock)
{
    return reinterpret_cast<queued_spinlock_token_t*>(spinlock & QueuedSpinlockPointer);
}

static inline std::uintptr_t Val(queued_spinlock_token_t* token)
{
    return reinterpret_cast<std::uintptr_t>(token);
}

static inline std::uintptr_t LockedVal(queued_spinlock_token_t* token)
{
    return Val(token) | QueuedSpinlockLocked;
}


void QueuedSpinlockAcquire(queued_spinlock_t* spinlock, queued_spinlock_token_t* token)
{
    // Fast path: attempt to lock the spinlock assuming that it is uncontended.
    //
    // Because we're hoping it's uncontended, we don't add our token to the queue on this path. We also do a weak
    // compare-exchange as a failure in the interlocked operation suggests contention so there's a good chance it'd need
    // the slow path anyway.
    std::atomic_ref lockword(spinlock->opaque);
    auto desired = QueuedSpinlockLocked;
    std::uintptr_t current = 0;
    if (lockword.compare_exchange_weak(current, desired, std::memory_order::acquire, std::memory_order_relaxed)) [[likely]]
        return;

    // Clear the token - this needs to happen before any update to the spinlock's value.
    std::atomic_ref tokenword(token->opaque);
    tokenword.store(QueuedSpinlockLocked, std::memory_order::relaxed);

    // Insert our token at the tail of the queue of waiters for this lock.
    current = lockword.exchange(LockedVal(token), std::memory_order_release);

    // Check to see whether we actually managed to get ownership of the lock.
    if (!IsLocked(current))
        return;

    // The pointer stored in the lock is to the last waiter in the queue. Because of the exchange operation, we know
    // that no other thread is going to be modifying this waiter's token.
    auto waitertoken = GetNext(current);
    std::atomic_ref waitertokenword(waitertoken->opaque);
    auto flags = waitertokenword.load(std::memory_order::relaxed) & QueuedSpinlockFlags;
    waitertokenword.store(Val(token) | flags, std::memory_order::relaxed);

    // We're now ready to spin. This is going to be done on our own private locked bit rather than the one in the main
    // spinlock -- this avoids a lot of cache line contention on that main lock.
    while (IsLocked(tokenword.load(std::memory_order::relaxed)))
    {
        [[unlikely]];   
        SPINLOCK_YIELD_HINT;
    }

    // We now have ownership of the lock. We impose an acquire here to give us the same barrier as on the fast path.
    std::atomic_thread_fence(std::memory_order::acquire);
}


void QueuedSpinlockRelease(queued_spinlock_t* spinlock, queued_spinlock_token_t* token)
{
    // Fast path: there are no waiters, just clear the lock bit.
    std::atomic_ref lockword(spinlock->opaque);
    std::uintptr_t desired = 0;
    std::uintptr_t current = QueuedSpinlockLocked;
    if (lockword.compare_exchange_weak(current, desired, std::memory_order::release, std::memory_order_relaxed)) [[likely]]
        return;

    // Medium path: we were the only waiter on the lock.
    while (GetNext(current) == token)
    {
        // If we are the last waiter and can atomically remove our token from the queue, it means that no other thread
        // can be potentially accessing the token and no special behaviour is needed to wait until they're done.
        if (lockword.compare_exchange_weak(current, desired, std::memory_order::release, std::memory_order::relaxed))
            return;
    }

    // Slow path: we're not the last waiter. This means that some other thread may have access to the token. As the
    // token was created with a null next pointer, we just need to wait until the single write that sets the next
    // pointer has completed. Until then, we can't free the token (as there is a pending async write to it) and we don't
    // know the address that the next waiter is spinning on, anyway.
    //
    //! @todo: validate that the control flow dependency here is enough to impose an ordering between this relaxed read
    //!        and the subsequent write through that pointer.
    std::atomic_ref tokenword(token->opaque);
    queued_spinlock_token_t* next = nullptr;
    while (!(next = GetNext(tokenword.load(std::memory_order::relaxed))))
    {
        [[unlikely]];
        SPINLOCK_YIELD_HINT;
    }

    // Unset the lock bit in the next waiter so it can break from its spin.
    std::atomic_ref nextwaiterword(next->opaque);
    desired = 0;
    current = QueuedSpinlockLocked;
    while (!nextwaiterword.compare_exchange_weak(current, desired, std::memory_order::release, std::memory_order::relaxed))
        desired = current & ~QueuedSpinlockLocked;
}


} // namespace System::Concurrency

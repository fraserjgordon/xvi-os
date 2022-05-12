#ifndef __SYSTEM_CONCURRENCY_SPINLOCK_H
#define __SYSTEM_CONCURRENCY_SPINLOCK_H


#include <System/Concurrency/Private/Config.hh>

#include <cstdint>


namespace System::Concurrency
{


struct queued_spinlock_t { std::uintptr_t opaque = 0; };

struct queued_spinlock_token_t { std::uintptr_t opaque = 0; };

__SYSTEM_CONCURRENCY_EXPORT void QueuedSpinlockAcquire(queued_spinlock_t*, queued_spinlock_token_t*) __SYSTEM_CONCURRENCY_SYMBOL(QueuedSpinlockAcquire);
__SYSTEM_CONCURRENCY_EXPORT void QueuedSpinlockRelease(queued_spinlock_t*, queued_spinlock_token_t*) __SYSTEM_CONCURRENCY_SYMBOL(QueuedSpinlockRelease);


} // namespace System::Concurrency


#endif /* ifndef __SYSTEM_CONCURRENCY_SPINLOCK_H */

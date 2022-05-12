#ifndef __SYSTEM_CONCURRENCY_SYNCWORD_H
#define __SYSTEM_CONCURRENCY_SYNCWORD_H


#include <System/Concurrency/Private/Config.hh>


namespace System::Concurrency
{


__SYSTEM_CONCURRENCY_EXPORT void SyncWordWait(void* word, const void* expect, int size) __SYSTEM_CONCURRENCY_SYMBOL(SyncWordWait);
__SYSTEM_CONCURRENCY_EXPORT void SyncWordWake(void* word, const void* value, int size, bool all) __SYSTEM_CONCURRENCY_SYMBOL(SyncWordWake);

__SYSTEM_CONCURRENCY_EXPORT void SharedSyncWordWait(void* word, const void* expect, int size) __SYSTEM_CONCURRENCY_SYMBOL(SharedSyncWordWait);
__SYSTEM_CONCURRENCY_EXPORT void SharedSyncWordWake(void* word, const void* value, int size, bool all) __SYSTEM_CONCURRENCY_SYMBOL(SharedSyncWordWake);


} // namespace System::Concurrency


#endif /* ifndef __SYSTEM_CONCURRENCY_SYNCWORD_H */

#ifndef __SYSTEM_IPC_SYNC_WAITONADDRESS_H
#define __SYSTEM_IPC_SYNC_WAITONADDRESS_H


#include <System/IPC/Sync/Private/Config.hh>

#include <chrono>
#include <cstddef>


namespace System::IPC::Sync
{


__SYSTEM_IPC_SYNC_EXPORT
bool WaitOnAddressUntil(void* address, const void* compare, std::size_t size, std::chrono::steady_clock::time_point until) __SYSTEM_IPC_SYNC_SYMBOL(WaitOnAddressUntil);

__SYSTEM_IPC_SYNC_EXPORT
bool WaitOnAddressFor(void* address, const void* compare, std::size_t, std::chrono::steady_clock::duration duration) __SYSTEM_IPC_SYNC_SYMBOL(WaitOnAddressFor);


template <class T>
static inline bool WaitOnAddressUntil(T* address, T compare, std::chrono::steady_clock::time_point until)
{
    return WaitOnAddressUntil(address, &compare, sizeof(T), until);
}

template <class T>
static inline bool WaitOnAddressFor(T* address, T compare, std::chrono::steady_clock::duration duration)
{
    return WaitOnAddressFor(address, &compare, sizeof(T), duration);
}


} // namespace System.IPC.Sync


#endif /* ifndef __SYSTEM_IPC_SYNC_WAITONADDRESS_H */

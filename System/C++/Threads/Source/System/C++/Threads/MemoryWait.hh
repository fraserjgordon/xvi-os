#pragma once
#ifndef __SYSTEM_CXX_THREADS_MEMORYWAIT_H
#define __SYSTEM_CXX_THREADS_MEMORYWAIT_H


#include <System/C++/LanguageSupport/StdDef.hh>


// TODO: remove me.
extern "C" [[gnu::weak]] void waitOnAddress(void* where, const void* compare, std::size_t size) asm("Syscall.Sync.WaitOnAddress");
extern "C" [[gnu::weak]] void notifyAddress(void* where, bool all) asm("Syscall.Sync.NotifyAddress");



#endif /* ifndef __SYSTEM_CXX_THREADS_MEMORYWAIT_H */

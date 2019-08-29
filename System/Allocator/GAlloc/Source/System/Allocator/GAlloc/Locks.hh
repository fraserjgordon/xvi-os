#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_LOCKS_H
#define __SYSTEM_ALLOCATOR_GALLOC_LOCKS_H


#include <System/C++/Atomic/Atomic.hh>

#include <System/Allocator/GAlloc/Config.hh>


namespace __GALLOC_NS_DECL
{


class NullGuard
{
public:

    void release() {}
    ~NullGuard() { release(); }
};


class Spinlock
{
public:

    using Guard = NullGuard;

    [[nodiscard]] Guard lock();
    bool tryLock();
    void unlock();

private:

    std::atomic_flag m_flag;
};


class Mutex
{
public:

    using Guard = NullGuard;

    [[nodiscard]] Guard lock();
    bool tryLock();
    void unlock();

private:
};


using Lock = std::conditional_t<KernelMode, Spinlock, Mutex>;


} // namespace __GALLOC_NS_DECL


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_LOCKS_H */

#ifndef __SYSTEM_CXX_THREADS_MUTEX_H
#define __SYSTEM_CXX_THREADS_MUTEX_H


#include <System/C++/LanguageSupport/StdInt.hh> 

#include <System/C++/Threads/Private/Config.hh>
#include <System/C++/Threads/Lock.hh>


namespace __XVI_STD_THREADS_NS
{


class mutex
{
public:

    using native_handle_type    = void*;

    constexpr mutex() noexcept = default;

    ~mutex()
    {
        mutexDestroy(_M_val);
    }

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    void lock()
    {
        mutexLock(_M_val);
    }

    bool try_lock()
    {
        return mutexTryLock(_M_val);
    }

    void unlock()
    {
        mutexUnlock(_M_val);
    }

    native_handle_type native_handle()
    {
        return &_M_val;
    }

private:

    std::uintptr_t  _M_val = 0;


    __XVI_CXX_THREADS_EXPORT static void mutexLock(std::uintptr_t&) __XVI_CXX_THREADS_SYMBOL(Mutex.Lock);
    __XVI_CXX_THREADS_EXPORT static bool mutexTryLock(std::uintptr_t&) __XVI_CXX_THREADS_SYMBOL(Mutex.TryLock);
    __XVI_CXX_THREADS_EXPORT static void mutexUnlock(std::uintptr_t&) __XVI_CXX_THREADS_SYMBOL(Mutex.Unlock);
    __XVI_CXX_THREADS_EXPORT static void mutexDestroy(std::uintptr_t&) __XVI_CXX_THREADS_SYMBOL(Mutex.Destroy);
};


} // namespace __XVI_STD_THREADS_NS


#endif /* ifndef __SYSTEM_CXX_THREADS_MUTEX_H */

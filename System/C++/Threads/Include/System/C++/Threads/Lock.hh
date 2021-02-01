#ifndef __SYSTEM_CXX_THREADS_LOCK_H
#define __SYSTEM_CXX_THREADS_LOCK_H


#include <System/C++/Utility/Chrono.hh>
#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/SystemError.hh>
#include <System/C++/Utility/Tuple.hh>

#include <System/C++/Threads/Private/Config.hh>


namespace __XVI_STD_THREADS_NS
{


struct defer_lock_t { explicit defer_lock_t() = default; };
struct try_lock_t { explicit try_lock_t() = default; };
struct adopt_lock_t { explicit adopt_lock_t() = default; };


inline constexpr defer_lock_t defer_lock {};
inline constexpr try_lock_t try_to_lock {};
inline constexpr adopt_lock_t adopt_lock {};


template <class _Mutex>
class lock_guard
{
public:

    using mutex_type    = _Mutex;

    explicit lock_guard(mutex_type& __m) :
        _M_mutex(__m)
    {
        _M_mutex.lock();
    }

    lock_guard(mutex_type& __m, adopt_lock_t) noexcept :
        _M_mutex(__m)
    {
    }

    ~lock_guard()
    {
        _M_mutex.unlock();
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:

    mutex_type& _M_mutex;
};


namespace __detail
{

template <class...>
struct __scoped_lock_base {};

template <class _M>
struct __scoped_lock_base<_M> { using mutex_type = _M; };

} // namespace __detail


template <class... _MutexTypes>
class scoped_lock :
    public __detail::__scoped_lock_base<_MutexTypes...>
{
public:

    explicit scoped_lock(_MutexTypes&... __m) :
        _M_m(tie(__m...))
    {
        if constexpr (sizeof...(__m) == 1)
            (__m.lock(), ...);
        else if constexpr (sizeof...(__m) > 1)
            lock(__m...);
    }

    explicit scoped_lock(adopt_lock_t, _MutexTypes&... __m) noexcept :
        _M_m(tie(__m...))
    {
    }

    ~scoped_lock()
    {
        apply([](_MutexTypes&... __m)
        {
            (__m.unlock(), ...);
        }, _M_m);
    }

    scoped_lock(const scoped_lock&) = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;

private:

    tuple<_MutexTypes&...>  _M_m;
};

template <class _Mutex>
class unique_lock
{
public:

    using mutex_type    = _Mutex;

    unique_lock() noexcept
    {
    }

    explicit unique_lock(mutex_type& __m) :
        _M_mutex(addressof(__m))
    {
        _M_mutex->lock();
        _M_owns = true;
    }

    unique_lock(mutex_type& __m, defer_lock_t) noexcept :
        _M_mutex(addressof(__m))
    {
    }

    unique_lock(mutex_type& __m, try_lock_t) :
        _M_mutex(addressof(__m))
    {
        _M_owns = _M_mutex->try_lock();
    }

    unique_lock(mutex_type& __m, adopt_lock_t) noexcept :
        _M_mutex(addressof(__m)),
        _M_owns(true)
    {
    }

    template <class _Clock, class _Duration>
    unique_lock(mutex_type& __m, const chrono::time_point<_Clock, _Duration>& __abs_time) :
        _M_mutex(addressof(__m))
    {
        _M_owns = _M_mutex->try_lock_until(__abs_time);
    }

    template <class _Rep, class _Period>
    unique_lock(mutex_type& __m, const chrono::duration<_Rep, _Period>& __rel_time) :
        _M_mutex(addressof(__m))
    {
        _M_owns = _M_mutex->try_lock_for(__rel_time);
    }

    ~unique_lock()
    {
        if (_M_owns)
            _M_mutex->unlock();
    }

    unique_lock(const unique_lock&) = delete;
    unique_lock& operator=(const unique_lock&) = delete;

    unique_lock(unique_lock&& __u) noexcept :
        _M_mutex(__u._M_mutex),
        _M_owns(__u._M_owns)
    {
        __u._M_mutex = nullptr;
        __u._M_owns = false;
    }

    unique_lock& operator=(unique_lock&& __u) noexcept
    {
        if (_M_owns)
            _M_mutex->unlock();

        _M_mutex = __u._M_mutex;
        _M_owns = __u._M_owns;

        __u._M_mutex = nullptr;
        __u._M_owns = false;

        return *this;
    }

    void lock()
    {
        if (_M_mutex == nullptr)
            throw system_error(make_error_code(errc::operation_not_permitted));
        else if (_M_owns)
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));

        _M_mutex->lock();
    }

    bool try_lock()
    {
        if (_M_mutex == nullptr)
            throw system_error(make_error_code(errc::operation_not_permitted));
        else if (_M_owns)
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));

        _M_owns = _M_mutex->try_lock();
    }

    template <class _Clock, class _Duration>
    bool try_lock_until(const chrono::time_point<_Clock, _Duration>& __abs_time)
    {
        if (_M_mutex == nullptr)
            throw system_error(make_error_code(errc::operation_not_permitted));
        else if (_M_owns)
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));

        return _M_owns = _M_mutex->try_lock_until(__abs_time);
    }

    template <class _Rep, class _Period>
    bool try_lock_for(const chrono::duration<_Rep, _Period>& __rel_time)
    {
        if (_M_mutex == nullptr)
            throw system_error(make_error_code(errc::operation_not_permitted));
        else if (_M_owns)
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));

        return _M_owns = _M_mutex->try_lock_for(__rel_time);
    }

    void unlock()
    {
        if (!_M_owns)
            throw system_error(make_error_code(errc::operation_not_permitted));

        _M_mutex->unlock();
    }

    void swap(unique_lock& __u) noexcept
    {
        swap(_M_mutex, __u._M_mutex);
        swap(_M_owns, __u._M_owns);
    }

    mutex_type* release() noexcept
    {
        auto __m = _M_mutex;
        _M_mutex = nullptr;
        _M_owns = false;
        return __m;
    }

    bool owns_lock() const noexcept
    {
        return _M_owns;
    }

    explicit operator bool() const noexcept
    {
        return owns_lock();
    }

    mutex_type* mutex() const noexcept
    {
        return _M_mutex;
    }

private:

    mutex_type* _M_mutex    = nullptr;
    bool        _M_owns     = false;
};

template <class _Mutex>
void swap(unique_lock<_Mutex>& __x, unique_lock<_Mutex>& __y)
{
    __x.swap(__y);
}

template <class _L1, class _L2, class... _L3>
int try_lock(_L1& __l1, _L2& __l2, _L3&... __l3)
{
    int __index = 0;
    auto __lock = [&__index](auto& __l) -> bool
    {
        auto __locked = __l.try_lock();
        if (__locked)
            ++__index;
        return __locked;
    };

    bool __all = __lock(__l1) && __lock(__l2) && (__lock(__l3) && ...); 

    if (__all)
        return -1;
    else
        return __index;
}


namespace __detail
{


template <class _L, class... _Ls>
bool __do_lock(_L& __l, _Ls&... __ls)
{
    if (!__l.try_lock())
        return false;

    if constexpr (sizeof...(__ls) > 0)
    {
        try
        {
            bool __success = __do_lock(__ls...);
            if (!__success)
                __l.unlock();
            return __success;
        }
        catch (...)
        {
            __l.unlock();
            throw;
        }
    }
}


} // namespace __detail


template <class _L1, class _L2, class... _L3>
void lock(_L1& __l1, _L2& __l2, _L3&... __l3)
{
    while (true)
    {
        // Blocking lock attempt on the first one.
        __l1.lock();

        // Non-blocking locks on the rest. We want to optimise for the success case.
        if (__detail::__do_lock(__l2, __l3...)) [[likely]]
            return;

        // Release the main lock and try again.
        __l1.unlock();
    }
}


} // namespace __XVI_STD_THREADS_NS


#endif /* ifndef __SYSTEM_CXX_THREADS_LOCK_H */

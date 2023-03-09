#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COROUTINE_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_COROUTINE_H


#include <System/C++/LanguageSupport/Private/Config.hh>
#include <System/C++/LanguageSupport/Compare.hh>


namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


template <class _R, class... _Args> struct coroutine_traits {};

template <class _R, class... _Args>
    requires requires { typename _R::promise_type; }
struct coroutine_traits<_R, _Args...>
{
    using promise_type = typename _R::promise_type;
};


template <class _Promise = void>
class coroutine_handle;

template <>
class coroutine_handle<void>
{
public:

    constexpr coroutine_handle() noexcept = default;
    constexpr coroutine_handle(const coroutine_handle&) noexcept = default;
    constexpr coroutine_handle(coroutine_handle&&) noexcept = default;

    constexpr coroutine_handle& operator=(const coroutine_handle&) noexcept = default;
    constexpr coroutine_handle& operator=(coroutine_handle&&) noexcept = default;

    constexpr ~coroutine_handle() = default;

    constexpr coroutine_handle(nullptr_t) noexcept :
        coroutine_handle()
    {
    }

    coroutine_handle& operator=(nullptr_t) noexcept
    {
        _M_ptr = nullptr;
        return *this;
    }

    constexpr void* address() const noexcept
    {
        return _M_ptr;
    }

    constexpr explicit operator bool() const noexcept
    {
        return _M_ptr != nullptr;
    }

    bool done() const
    {
        return __builtin_coro_done(_M_ptr);
    }

    void resume() const
    {
        __builtin_coro_resume(_M_ptr);
    }

    void destroy() const
    {
        __builtin_coro_destroy(_M_ptr);
    }

    void operator()() const
    {
        resume();
    }


    static constexpr coroutine_handle from_address(void* __addr)
    {
        coroutine_handle __handle;
        __handle._M_ptr = __addr;
        return __handle;
    }

private:

    void* _M_ptr = nullptr;
};

template <class _Promise>
class coroutine_handle
{
public:

    constexpr coroutine_handle() noexcept = default;
    constexpr coroutine_handle(const coroutine_handle&) noexcept = default;
    constexpr coroutine_handle(coroutine_handle&&) noexcept = default;

    constexpr coroutine_handle& operator=(const coroutine_handle&) noexcept = default;
    constexpr coroutine_handle& operator=(coroutine_handle&&) noexcept = default;

    constexpr ~coroutine_handle() = default;

    constexpr coroutine_handle(nullptr_t) noexcept :
        coroutine_handle()
    {
    }

    coroutine_handle& operator=(nullptr_t) noexcept
    {
        _M_ptr = nullptr;
        return *this;
    }

    constexpr void* address() const noexcept
    {
        return _M_ptr;
    }

    constexpr operator coroutine_handle<>() const noexcept
    {
        return coroutine_handle<>::from_address(address());
    }

    constexpr explicit operator bool() const noexcept
    {
        return _M_ptr != nullptr;
    }

    bool done() const
    {
        return __builtin_coro_done(_M_ptr);
    }

    void resume() const
    {
        __builtin_coro_resume(_M_ptr);
    }

    void destroy() const
    {
        __builtin_coro_destroy(_M_ptr);
    }

    void operator()() const
    {
        resume();
    }

    _Promise& promise() const
    {
        return *reinterpret_cast<_Promise*>(__builtin_coro_promise(_M_ptr, alignof(_Promise), false));
    }


    static coroutine_handle from_promise(const _Promise& __p)
    {
        coroutine_handle __handle;
        __handle._M_ptr = __builtin_coro_promise(std::addressof(__p), alignof(_Promise), true);
        return __handle;
    }

    static constexpr coroutine_handle from_address(void* __addr)
    {
        coroutine_handle __handle;
        __handle._M_ptr = __addr;
        return __handle;
    }

private:

    void* _M_ptr = nullptr;
};


constexpr bool operator==(coroutine_handle<> __x, coroutine_handle<> __y) noexcept
{
    return __x.address() == __y.address();
}

constexpr std::strong_ordering operator<=>(coroutine_handle<> __x, coroutine_handle<> __y) noexcept
{
    return compare_three_way()(__x.address(), __y.address());
}


template <class> struct hash;

template <class _P>
struct hash<coroutine_handle<_P>>;


struct noop_coroutine_promise {};


namespace __detail
{


// Common ABI for coroutine frames.
struct __coroutine_frame
{
    using __resume_fn = void (*)(__coroutine_frame*);
    using __destroy_fn = void (*)(__coroutine_frame*);

    __resume_fn __resume = nullptr;
    __destroy_fn __destroy = nullptr;
};


constexpr void __noop_coroutine_resume(__coroutine_frame*)
{
}

constexpr void __noop_coroutine_destroy(__coroutine_frame*)
{
}

// Hand-constructed coroutine frame for no-op coroutines.
struct __noop_coroutine_frame
{
    __coroutine_frame::__resume_fn __resume = &__noop_coroutine_resume;
    __coroutine_frame::__destroy_fn __destroy = &__noop_coroutine_destroy;

    noop_coroutine_promise __promise = {};
};


// The standard permits distinct instances of no-op coroutines to share the same frame address.
inline __noop_coroutine_frame __noop_coroutine = {};


} // namespace __detail


template <>
class coroutine_handle<noop_coroutine_promise>
{
public:

    constexpr operator coroutine_handle<>() const noexcept
    {
        return coroutine_handle<>::from_address(address());
    }

    constexpr explicit operator bool() const noexcept
    {
        return true;
    }

    constexpr bool done() const noexcept
    {
        return false;
    }

    constexpr void resume() const noexcept
    {
    }

    constexpr void destroy() const noexcept
    {
    }

    constexpr void operator()() const noexcept
    {
    }

    noop_coroutine_promise& promise() const noexcept
    {
        return reinterpret_cast<__detail::__noop_coroutine_frame*>(_M_ptr)->__promise;
    }

    constexpr void* address() const noexcept
    {
        return _M_ptr;
    }


    static coroutine_handle __make()
    {
        coroutine_handle __handle;
        __handle._M_ptr = &__detail::__noop_coroutine;
        return __handle;
    }

private:

    void* _M_ptr = nullptr;
};

using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;

inline noop_coroutine_handle noop_coroutine() noexcept
{
    return noop_coroutine_handle::__make();
}


struct suspend_never
{
    constexpr bool await_ready() const noexcept
    {
        return true;
    }

    constexpr void await_suspend(coroutine_handle<>) const noexcept
    {
    }

    constexpr void await_resume() const noexcept
    {
    }
};


struct suspend_always
{
    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    constexpr void await_suspend(coroutine_handle<>) const noexcept
    {
    }

    constexpr void await_resume() const noexcept
    {
    }
};


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COROUTINE_H */

#ifndef __SYSTEM_CXX_UTILITY_GENERATOR_H
#define __SYSTEM_CXX_UTILITY_GENERATOR_H


#include <System/C++/Containers/Stack.hh>
#include <System/C++/LanguageSupport/Coroutine.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/UniquePtr.hh>

#include <System/C++/Coroutine/Private/Config.hh>


namespace __XVI_STD_COROUTINE_NS
{


template <class _Ref, class _V = void, class _Allocator = void>
class generator :
    public ranges::view_interface<generator<_Ref, _V, _Allocator>>
{
private:

    using __value_t = __XVI_STD_NS::conditional_t<__XVI_STD_NS::is_void_v<_V>, __XVI_STD_NS::remove_cvref_t<_Ref>, _V>;
    using __reference_t = __XVI_STD_NS::conditional_t<__XVI_STD_NS::is_void_v<_V>, _Ref&&, _Ref>;

    class __iterator;

public:

    using yielded = __XVI_STD_NS::conditional_t<__XVI_STD_NS::is_reference_v<__reference_t>, __reference_t, const __reference_t&>;

    class promise_type;

    generator(const generator&) = delete;

    generator(generator&& __other) noexcept :
        _M_coroutine(exchange(__other._M_coroutine, {})),
        _M_active(exchange(__other._M_active, nullptr))
    {
    }

    ~generator()
    {
        if (_M_coroutine)
            _M_coroutine.destroy();
    }

    generator& operator=(generator __other) noexcept
    {
        swap(_M_coroutine, __other._M_coroutine);
        swap(_M_active, __other._M_active);
        return *this;
    }

    __iterator begin();

    default_sentinel_t end() const noexcept
    {
        return default_sentinel;
    }

private:

    coroutine_handle<promise_type>          _M_coroutine = nullptr;
    unique_ptr<stack<coroutine_handle<>>>   _M_active;
};


template <class _Ref, class _V, class _Allocator>
class generator<_Ref, _V, _Allocator>::promise_type
{
public:

    generator get_return_object() noexcept;

    suspend_always initial_suspend() const noexcept
    {
        return {};
    }

    auto final_suspend() noexcept;

    suspend_always yield_value(yielded __val) noexcept;

    auto yield_value(const __XVI_STD_NS::remove_reference_t<yielded>& __val)
        requires __XVI_STD_NS::is_rvalue_reference_v<yielded>
                 && __XVI_STD_NS::constructible_from<__XVI_STD_NS::remove_cvref_t<yielded>, const __XVI_STD_NS::remove_reference_t<yielded>&>;

    template <class _R2, class _V2, class _Alloc2, class _Unused>
        requires __XVI_STD_NS::same_as<typename generator<_R2, _V2, _Alloc2>::yielded, yielded>
    auto yield_value(ranges::elements_of<generator<_R2, _V2, _Alloc2>&&, _Unused> __g) noexcept;

    template <ranges::input_range _R, class _Alloc>
        requires __XVI_STD_NS::convertible_to<ranges::range_reference_t<_R>, yielded>
    auto yield_value(ranges::elements_of<_R, _Alloc> __r) noexcept;

    void await_transform() = delete;

    void return_void() const noexcept
    {
    }

    void unhandled_exception();

    void* operator new(size_t __size)
        requires __XVI_STD_NS::same_as<_Allocator, void> || __XVI_STD_NS::default_initializable<_Allocator>;

    template <class _Alloc, class... _Args>
        requires __XVI_STD_NS::same_as<_Allocator, void> || __XVI_STD_NS::convertible_to<const _Alloc&, _Allocator>
    void* operator new(size_t __size, allocator_arg_t, const _Alloc& __alloc, const _Args&... __args);

    template <class _This, class _Alloc, class... _Args>
        requires __XVI_STD_NS::same_as<_Allocator, void> || __XVI_STD_NS::convertible_to<const _Alloc&, _Allocator>
    void* operator new(size_t __size, const _This& __this, allocator_arg_t, const _Alloc& __alloc, const _Args&... __args);

    void operator delete(void* __ptr, size_t __size) noexcept;

private:

    __XVI_STD_NS::add_pointer_t<yielded>    _M_value = nullptr;
    exception_ptr                           _M_except;
};


template <class _Ref, class _V, class _Allocator>
class generator<_Ref, _V, _Allocator>::__iterator
{
public:

    using value_type        = __value_t;
    using difference_type   = ptrdiff_t;

    __iterator(__iterator&& __other) noexcept :
        _M_coroutine(__XVI_STD_NS::exchange(__other._M_coroutine, {}))
    {
    }

    __iterator& operator=(__iterator&& __other) noexcept
    {
        _M_coroutine = __XVI_STD_NS::exchange(__other._M_coroutine, {});
        return *this;
    }

    __reference_t operator*() const noexcept(__XVI_STD_NS::is_nothrow_copy_constructible_v<__reference_t>);

    __iterator& operator++();

    void operator++(int)
    {
        ++*this;
    }

    friend bool operator==(const __iterator& __i, default_sentinel_t)
    {
        return __i._M_coroutine.done();
    }

private:

    coroutine_handle<promise_type> _M_coroutine;
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_GENERATOR_H */

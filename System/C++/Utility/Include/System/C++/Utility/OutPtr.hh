#ifndef __SYSTEM_CXX_UTILITY_OUTPTR_H
#define __SYSTEM_CXX_UTILITY_OUTPTR_H


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/Concepts.hh>
#include <System/C++/Utility/PointerTraits.hh>
#include <System/C++/Utility/SharedPtr.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{


template <class> struct __is_shared_ptr_specialization : false_type {};

template <class _T> struct __is_shared_ptr_specialization<shared_ptr<_T>> : true_type {};

template <class _T>
inline constexpr bool __is_shared_ptr_specialization_v = __is_shared_ptr_specialization<_T>::value;

template <class _Smart, class _Pointer, class... _Args>
concept __out_ptr_reset_valid = requires (_Smart& __s, _Pointer __p, _Args&&... __args)
    {
        __s.reset(static_cast<__pointer_of_or_t<_Smart, _Pointer>>(__p), std::forward<_Args>(__args)...);
    };


struct __out_ptr_monostate
{
    constexpr __out_ptr_monostate() = default;
    template <class... _Args> __out_ptr_monostate(_Args&&...) {};
};

template <bool __is_shared_ptr> using __out_ptr_prealloc_t = __XVI_STD_NS::conditional_t<__is_shared_ptr, unique_ptr<__shared_ptr_owner_base>, __out_ptr_monostate>;
template <bool __is_shared_ptr, class... _Args> using __out_ptr_args_t = __XVI_STD_NS::conditional_t<__is_shared_ptr, __out_ptr_monostate, tuple<_Args...>>;


} // namespace __detail


template <class _Smart, class _Pointer, class... _Args>
class out_ptr_t
{
    static constexpr bool __is_shared_ptr = __detail::__is_shared_ptr_specialization_v<_Smart>;
    using __prealloc_t = __detail::__out_ptr_prealloc_t<__is_shared_ptr>;
    using __args_t = __detail::__out_ptr_args_t<__is_shared_ptr, _Args...>;

public:

    static_assert(!__is_shared_ptr || sizeof...(_Args) > 0,
        "when using out_ptr on shared_ptr, a deleter argument must be given");

    explicit out_ptr_t(_Smart& __smart, _Args... __args) :
        _M_out{__smart},
        _M_args{std::forward<_Args>(__args)...}
    {
        if constexpr (__is_shared_ptr)
        {
            // We can check that the given args are valid for constructing a shared_ptr.
            static_assert(__XVI_STD_NS::is_constructible_v<_Smart, __XVI_STD_NS::nullptr_t, _Args...>,
                "arguments must be valid for constructing a shared_ptr");

            // Pre-allocate a shared pointer control structure to avoid any potentially-throwing memory allocations in
            // the destructor (if we delayed creation of the shared_ptr control block until then).
            //
            // Because the deleter and allocator are stored in the control block, we can supply them now and don't need
            // to store them for later use.
            _M_prealloc.reset(new __detail::__shared_ptr_owner<false, typename _Smart::element_type>{nullptr, 0, std::forward<_Args>(__args)...});
        }
    }

    out_ptr_t(const out_ptr_t&) = delete;

    ~out_ptr_t()
    {
        using _SP = __detail::__pointer_of_or_t<_Smart, _Pointer>;

        if constexpr (__is_shared_ptr)
        {
            // Create the shared_ptr using the preallocated control block to avoid any actions that may throw (e.g.
            // allocating the memory for the control block).
            if (_M_p)
            {
                // Always one element because the deleter is responsible for correctly dealing with arrays.
                _M_prealloc->__set(_M_p, 1);
                _M_out = _Smart::__make(_M_p, _M_prealloc.release());
            }
        }
        else if constexpr (__detail::__out_ptr_reset_valid<_Smart, _Pointer, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_out.reset(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (std::is_constructible_v<_Smart, _SP, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_out = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else
        {
            static_assert(!std::is_same_v<_Smart, _Smart>, "invalid template args for out_ptr");
        }
    }

    operator _Pointer*() const noexcept
    {
        return std::addressof(const_cast<_Pointer&>(_M_p));
    }

    operator void**() const noexcept
        requires (!std::is_same_v<_Pointer, void*>)
    {
        static_assert(std::is_pointer_v<_Pointer>);

        return reinterpret_cast<void**>(static_cast<_Pointer*>(*this));
    }

private:

    _Smart&         _M_out;
    _Pointer        _M_p {};
    [[no_unique_address]] __prealloc_t _M_prealloc {};
    [[no_unique_address]] __args_t _M_args {};
};


template <class _Pointer = void, class _Smart, class... _Args>
auto out_ptr(_Smart& __s, _Args&&... __args)
{
    using _P = std::conditional_t<std::is_void_v<_Pointer>, __detail::__pointer_of_t<_Smart>, _Pointer>;

    return out_ptr_t<_Smart, _P, _Args...>(__s, std::forward<_Args>(__args)...);
}


template <class _Smart, class _Pointer, class... _Args>
class inout_ptr_t
{
public:

    static_assert(!__detail::__is_shared_ptr_specialization_v<_Smart>,
        "shared_ptr cannot be used with inout_ptr");

    explicit inout_ptr_t(_Smart& __s, _Args... __args) 
        requires std::is_pointer_v<_Smart> :
        _M_inout{__s},
        _M_args{std::forward<_Args>(__args)...}
    {
    }

    explicit inout_ptr_t(_Smart& __s, _Args... __args)
        requires (!std::is_pointer_v<_Smart>) :
        _M_inout{__s},
        _M_args{std::forward<_Args>(__args)...}
    {
    }

    inout_ptr_t(const inout_ptr_t&) = delete;

    ~inout_ptr_t()
    {
        using _SP = __detail::__pointer_of_or_t<_Smart, _Pointer>;

        if constexpr (std::is_pointer_v<_Smart>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_inout = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (__detail::__out_ptr_reset_valid<_Smart, _Pointer, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_inout.release();
                    _M_inout.reset(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (std::is_constructible_v<_Smart, _SP, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_inout.release();
                    _M_inout = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else
        {
            static_assert(!std::is_same_v<_Smart, _Smart>, "invalid template args for inout_ptr");
        }
    }

    operator _Pointer*() const noexcept
    {
        return std::addressof(const_cast<_Pointer&>(_M_p));
    }

    operator void**() const noexcept
        requires (!std::is_same_v<_Pointer, void*>)
    {
        static_assert(std::is_pointer_v<_Pointer>);

        return reinterpret_cast<void**>(static_cast<_Pointer*>(*this));
    }

private:

    _Smart&         _M_inout;
    _Pointer        _M_p {};
    [[no_unique_address]] tuple<_Args...> _M_args {};
};


template <class _Pointer = void, class _Smart, class... _Args>
auto inout_ptr(_Smart& __s, _Args&&... __args)
{
    using _P = std::conditional_t<std::is_void_v<_Pointer>, __detail::__pointer_of_t<_Smart>, _Pointer>;

    return inout_ptr_t<_Smart, _P, _Args...>(__s, std::forward<_Args>(__args)...);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_OUTPTR_H */

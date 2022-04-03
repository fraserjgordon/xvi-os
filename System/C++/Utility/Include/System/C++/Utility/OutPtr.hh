#ifndef __SYSTEM_CXX_UTILITY_OUTPTR_H
#define __SYSTEM_CXX_UTILITY_OUTPTR_H


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/Concepts.hh>
#include <System/C++/Utility/PointerTraits.hh>
#include <System/C++/Utility/Tuple.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> class shared_ptr;


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


} // namespace __detail


template <class _Smart, class _Pointer, class... _Args>
class out_ptr_t
{
public:

    static_assert(!__detail::__is_shared_ptr_specialization_v<_Smart> || sizeof...(_Args) > 0,
        "when using out_ptr on shared_ptr, a deleter argument must be given");

    explicit out_ptr_t(_Smart& __smart, _Args... __args) :
        _M_smart{__smart},
        _M_args{std::forward<_Args>(__args)...},
        _M_p{}
    {
    }

    out_ptr_t(const out_ptr_t&) = delete;

    ~out_ptr_t()
    {
        using _SP = __detail::__pointer_of_or_t<_Smart, _Pointer>;

        if constexpr (__detail::__out_ptr_reset_valid<_Smart, _Pointer, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_smart.reset(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (std::is_constructible_v<_Smart, _SP, _Args...>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_smart = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
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

    _Smart&         _M_smart;
    tuple<_Args...> _M_args;
    _Pointer        _M_p;
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
        _M_smart{__s},
        _M_args{std::forward<_Args>(__args)...},
        _M_p{__s}
    {
    }

    explicit inout_ptr_t(_Smart& __s, _Args... __args)
        requires (!std::is_pointer_v<_Smart>) :
        _M_smart{__s},
        _M_args{std::forward<_Args>(__args)...},
        _M_p{__s.get()}
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
                    _M_smart = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (__detail::__out_ptr_reset_valid<_Smart, _Pointer, _Args>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_smart.release();
                    _M_smart.reset(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
                }, std::move(_M_args));
            }
        }
        else if constexpr (std::is_constructible_v<_Smart, _SP, _Args>)
        {
            if (_M_p)
            {
                apply([&](auto&&... __args)
                {
                    _M_smart.release();
                    _M_smart = _Smart(static_cast<_SP>(_M_p), std::forward<_Args>(__args)...);
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

    _Smart&         _M_smart;
    tuple<_Args...> _M_args;
    _Pointer        _M_p;
};


template <class _Pointer = void, class _Smart, class... _Args>
auto inout_ptr(_Smart& __s, _Args&&... __args)
{
    using _P = std::conditional_t<std::is_void_v<_Pointer>, __detail::__pointer_of_t<_Smart>, _Pointer>;

    return inout_ptr_t<_Smart, _P, _Args...>(__s, std::forward<_Args>(__args)...);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_OUTPTR_H */

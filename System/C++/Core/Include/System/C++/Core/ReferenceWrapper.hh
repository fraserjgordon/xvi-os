#ifndef __SYSTEM_CXX_CORE_REFERENCEWRAPPER_H
#define __SYSTEM_CXX_CORE_REFERENCEWRAPPER_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>

#include <System/C++/Core/AddressOf.hh>


namespace __XVI_STD_CORE_NS_DECL
{



namespace __detail
{

template <class _T> void __ref_wrapper_test_fn(_T&) noexcept;
template <class _T> void __ref_wrapper_test_fn(_T&&) = delete;

} // namespace __detail


template <class _T>
class reference_wrapper
{
public:

    using type = _T;

    template <class _U>
        requires requires { __detail::__ref_wrapper_test_fn(declval<_U>()); }
            && (!__XVI_STD_NS::is_same_v<__XVI_STD_NS::remove_cvref_t<_U>, reference_wrapper>)
    reference_wrapper(_U&& __u) 
        noexcept(noexcept(__detail::__ref_wrapper_test_fn(declval<_U>())))
        : _M_ptr([&__u]() mutable { _T& __r = __XVI_STD_NS::forward<_U>(__u); return __XVI_STD_NS::addressof(__r); }())
    {
    }

    reference_wrapper(const reference_wrapper& __x) noexcept = default;

    reference_wrapper& operator=(const reference_wrapper& __x) noexcept = default;

    operator _T& () const noexcept
    {
        return get();
    }

    _T& get() const noexcept
    {
        return *_M_ptr;
    }

    template <class... _ArgTypes>
    invoke_result_t<_T&, _ArgTypes...> operator() (_ArgTypes&&... __args) const
    {
        return invoke(get(), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

private:

    _T* _M_ptr;
};

template <class _T>
reference_wrapper(_T&) -> reference_wrapper<_T>;


template <class _T> reference_wrapper<_T> ref(_T& __t) noexcept
    { return reference_wrapper<_T>(__t); }

template <class _T> void ref(const _T&&) = delete; 

template <class _T> reference_wrapper<_T> ref(reference_wrapper<_T> __t) noexcept
    { return __t; }

template <class _T> reference_wrapper<const _T> cref(const _T& __t) noexcept
    { return reference_wrapper<const _T>(__t); }

template <class _T> void cref(const _T&&) = delete;

template <class _T> reference_wrapper<const _T> cref(reference_wrapper<_T> __t) noexcept
    { return __t; }


template <class _T> struct unwrap_reference { using type = _T; };
template <class _T> struct unwrap_reference<reference_wrapper<_T>> { using type = _T&; };

template <class _T> struct unwrap_ref_decay : unwrap_reference<__XVI_STD_NS::decay_t<_T>> {};
template <class _T> using unwrap_ref_decay_t = typename unwrap_ref_decay<_T>::type;


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_REFERENCEWRAPPER_H */
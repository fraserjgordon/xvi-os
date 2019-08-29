#pragma once
#ifndef __SYSTEM_CXX_UTILITY_REFERENCEWRAPPER_H
#define __SYSTEM_CXX_UTILITY_REFERENCEWRAPPER_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
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

    template <class _U, 
              class = enable_if_t<!is_same_v<remove_cvref_t<_U>, reference_wrapper>,
                                  decltype(__detail::__ref_wrapper_test_fn(declval<_U>()))>>
    reference_wrapper(_U&& __u) 
        noexcept(noexcept(__detail::__ref_wrapper_test_fn(declval<_U>())))
        : reference_wrapper(ref(__XVI_STD_NS::forward<_U>(__u))) {}

    reference_wrapper(const reference_wrapper& __x) noexcept
        : _M_ptr(__x._M_ptr)
    {
    }

    reference_wrapper& operator=(const reference_wrapper& __x) noexcept
    {
        _M_ptr = __x._M_ptr;
        return *this;
    }

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
        return __XVI_STD_TYPETRAITS_NS::__detail::_INVOKE(get(), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
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
    { return ref(__t.get()); }

template <class _T> reference_wrapper<const _T> cref(const _T& __t) noexcept
    { return reference_wrapper<const _T>(__t); }

template <class _T> void cref(const _T&&) = delete;

template <class _T> reference_wrapper<const _T> cref(reference_wrapper<_T> __t) noexcept
    { return cref(__t.get()); }


template <class _T> struct unwrap_reference { using type = _T; };
template <class _T> struct unwrap_reference<reference_wrapper<_T>> { using type = _T&; };

template <class _T> struct unwrap_ref_decay : unwrap_reference<decay_t<_T>> {};
template <class _T> using unwrap_ref_decay_t = typename unwrap_ref_decay<_T>::type;


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_REFERENCEWRAPPER_H */

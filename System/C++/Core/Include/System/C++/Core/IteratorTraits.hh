#ifndef __SYSTEM_CXX_CORE_ITERATORTRAITS_H
#define __SYSTEM_CXX_CORE_ITERATORTRAITS_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/PointerTraits.hh>
#include <System/C++/Core/Swap.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class _T> struct iterator_traits;


struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};


namespace __detail
{


#if 0
#define __DECAY(__x) auto(__x)
#else
#define __DECAY(__x) __XVI_STD_NS::decay_t<decltype(__x)>(__x)
#endif

template <class _T> using __with_reference = _T&;

template <class _T>
concept __can_reference = requires { typename __with_reference<_T>; };

template <class _T>
concept __dereferencable = requires(_T& __t)
{
    { *__t } -> __can_reference;
};


} // namespace __detail


template <__detail::__dereferencable _T>
using iter_reference_t = decltype(*declval<_T&>());


template <class> struct incrementable_traits {};

template <class _T>
    requires is_object_v<_T>
struct incrementable_traits<_T*>
{
    using difference_type = ptrdiff_t;
};

template <class _I>
struct incrementable_traits<const _I> : incrementable_traits<_I> {};

template <class _T>
    requires requires { typename _T::difference_type; }
struct incrementable_traits<_T>
{
    using difference_type = typename _T::difference_type;
};

template <class _T>
    requires (!requires { typename _T::difference_type; }
        && requires(const _T& __a, const _T& __b) { { __a - __b } -> integral; })
struct incrementable_traits<_T>
{
    using difference_type = make_signed_t<decltype(declval<_T>() - declval<_T>())>;
};


namespace __detail
{

// Not considered to be a primary iterator traits template if the special tag type is inherited from a base class.
template <class _T>
concept __is_primary_iterator_traits =
    requires { typename iterator_traits<_T>::__primary_iterator_traits_for; }
    && same_as<typename iterator_traits<_T>::__primary_iterator_traits_for, _T>;

template <class _T>
struct __iter_difference_t {};

template <class _T>
    requires (!__is_primary_iterator_traits<_T>) && __has_member_difference_type<iterator_traits<_T>>
struct __iter_difference_t<_T> { using __type = typename iterator_traits<_T>::difference_type; };

template <class _T>
    requires __is_primary_iterator_traits<_T> && __has_member_difference_type<incrementable_traits<_T>>
struct __iter_difference_t<_T> { using __type = typename incrementable_traits<_T>::difference_type; };

} // namespace __detail


template <class _T>
using iter_difference_t = typename __detail::__iter_difference_t<_T>::__type;


namespace __detail
{

template <class> struct __cond_value_type {};

template <class _T>
    requires is_object_v<_T>
struct __cond_value_type<_T>
{
    using value_type = remove_cv_t<_T>;
};

} // namespace __detail


template <class> struct indirectly_readable_traits {};

template <class _T>
struct indirectly_readable_traits<_T*> :
    __detail::__cond_value_type<_T> {};

template <class _I>
    requires is_array_v<_I>
struct indirectly_readable_traits<_I>
{
    using value_type = remove_cv_t<remove_extent_t<_I>>;
};

template <class _I>
struct indirectly_readable_traits<const _I> :
    indirectly_readable_traits<_I> {};

template <__detail::__has_member_value_type _T>
struct indirectly_readable_traits<_T> :
    __detail::__cond_value_type<typename _T::value_type> {};

template <__detail::__has_member_element_type _T>
struct indirectly_readable_traits<_T> :
    __detail::__cond_value_type<typename _T::element_type> {};

template <__detail::__has_member_value_type _T>
    requires __detail::__has_member_element_type<_T>
struct indirectly_readable_traits<_T> {};

template <__detail::__has_member_value_type _T>
    requires __detail::__has_member_element_type<_T>
        && same_as<remove_cv_t<typename _T::element_type>, remove_cv_t<typename _T::value_type>>
struct indirectly_readable_traits<_T> :
    __detail::__cond_value_type<typename _T::value_type> {};


namespace __detail
{

template <class _T>
struct __iter_value_t {};

template <class _T>
    requires (!__is_primary_iterator_traits<_T>) && __has_member_value_type<iterator_traits<_T>>
struct __iter_value_t<_T> { using __type = typename iterator_traits<_T>::value_type; };

template <class _T>
    requires __is_primary_iterator_traits<_T> && __has_member_value_type<indirectly_readable_traits<_T>>
struct __iter_value_t<_T> { using __type = typename indirectly_readable_traits<_T>::value_type; };

} // namespace __detail


template <class _T>
using iter_value_t = typename __detail::__iter_value_t<remove_cvref_t<_T>>::__type;


namespace __detail
{

template <class _I>
concept __cpp17_iterator =
    requires(_I __i)
    {
        {   *__i } -> __can_reference;
        {  ++__i } -> same_as<_I&>;
        { *__i++ } -> __can_reference;
    }
    && copyable<_I>;

template <class _I>
concept __cpp17_input_iterator = __cpp17_iterator<_I>
    && equality_comparable<_I>
    && requires(_I __i)
    {
        typename incrementable_traits<_I>::difference_type;
        typename indirectly_readable_traits<_I>::value_type;
        typename common_reference_t<iter_reference_t<_I>&&, typename indirectly_readable_traits<_I>::value_type&>;
        typename common_reference_t<decltype(*__i++)&&, typename indirectly_readable_traits<_I>::value_type&>;
        requires signed_integral<typename incrementable_traits<_I>::difference_type>;
    };

template <class _I>
concept __cpp17_forward_iterator = __cpp17_input_iterator<_I>
    && constructible_from<_I>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && same_as<remove_cvref_t<iter_reference_t<_I>>, typename indirectly_readable_traits<_I>::value_type>
    && requires(_I __i)
    {
        {  __i++ } -> convertible_to<const _I&>;
        { *__i++ } -> same_as<iter_reference_t<_I>>;
    };

template <class _I>
concept __cpp17_bidirectional_iterator = __cpp17_forward_iterator<_I>
    && requires(_I __i)
    {
        {  --__i } -> same_as<_I&>;
        {  __i-- } -> convertible_to<const _I&>;
        { *__i-- } -> same_as<iter_reference_t<_I>>;
    };

template <class _I>
concept __cpp17_random_access_iterator = __cpp17_bidirectional_iterator<_I>
    && totally_ordered<_I>
    && requires(_I __i, typename incrementable_traits<_I>::difference_type __n)
    {
        { __i += __n } -> same_as<_I&>;
        { __i -= __n } -> same_as<_I&>;
        { __i  + __n } -> same_as<_I>;
        { __n  + __i } -> same_as<_I>;
        { __i  - __n } -> same_as<_I>;
        { __i  - __i } -> same_as<decltype(__n)>;
        {  __i[__n]  } -> convertible_to<iter_reference_t<_I>>;
    };

} // namespace __detail


namespace __detail
{

template <class _I>
concept __has_iterator_types = requires
{
    typename _I::difference_type;
    typename _I::value_type;
    typename _I::reference;
    typename _I::iterator_category;
};

template <class _I>
concept __has_iterator_category = requires { typename _I::iterator_category; };

template <class _T>
concept __has_pointer_type = requires { typename _T::pointer; };

template <class _T>
concept __has_operator_arrow = requires(_T& __t) { __t.operator->(); };

template <class _T>
struct __iterator_traits_pointer { using __type = void; };

template <class _T>
    requires __has_pointer_type<_T>
struct __iterator_traits_pointer<_T> { using __type = typename _T::pointer; };

template <class _T>
    requires (!__has_pointer_type<_T> && !__has_iterator_types<_T> && __has_operator_arrow<_T>)
struct __iterator_traits_pointer<_T> { using __type = decltype(declval<_T&>().operator->()); };

template <class _T>
struct __iterator_traits_reference { using __type = iter_reference_t<_T>; };

template <class _T>
    requires requires { typename _T::reference; }
struct __iterator_traits_reference<_T> { using __type = typename _T::reference; };

template <class _I>
struct __iterator_traits_category;

template <class _I>
    requires __has_iterator_category<_I>
struct __iterator_traits_category<_I> { using __type = typename _I::iterator_category; };

template <class _I>
    requires __cpp17_random_access_iterator<_I>
        && (!__has_iterator_category<_I>)
struct __iterator_traits_category<_I> { using __type = random_access_iterator_tag; };

template <class _I>
    requires __cpp17_bidirectional_iterator<_I> 
        && (!__cpp17_random_access_iterator<_I>)
        && (!__has_iterator_category<_I>)
struct __iterator_traits_category<_I> { using __type = bidirectional_iterator_tag; };

template <class _I>
    requires __cpp17_forward_iterator<_I>
        && (!__cpp17_bidirectional_iterator<_I>)
        && (!__has_iterator_category<_I>)
struct __iterator_traits_category<_I> { using __type = forward_iterator_tag; };

template <class _I>
    requires __cpp17_input_iterator<_I>
        && (!__cpp17_forward_iterator<_I>)
        && (!__has_iterator_category<_I>)
struct __iterator_traits_category<_I> { using __type = input_iterator_tag; };

template <class _I>
struct __iterator_traits_difference_type { using __type = void; };

template <class _I>
    requires requires { typename _I::difference_type; }
struct __iterator_traits_difference_type<_I> { using __type = typename _I::difference_type; };

} // namespace __detail


template <class _I>
struct iterator_traits { using __primary_iterator_traits_for = _I; };

template <class _I>
    requires __detail::__has_iterator_types<_I>
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = typename _I::iterator_category;
    using value_type            = typename _I::value_type;
    using difference_type       = typename _I::difference_type;
    using pointer               = typename __detail::__iterator_traits_pointer<_I>::__type;
    using reference             = typename _I::reference;
};

template <class _I>
    requires (!__detail::__has_iterator_types<_I> && __detail::__cpp17_input_iterator<_I>)
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = typename __detail::__iterator_traits_category<_I>::__type;
    using value_type            = typename indirectly_readable_traits<_I>::value_type;
    using difference_type       = typename incrementable_traits<_I>::difference_type;
    using pointer               = typename __detail::__iterator_traits_pointer<_I>::__type;
    using reference             = typename __detail::__iterator_traits_reference<_I>::__type;
};

template <class _I>
    requires (!__detail::__has_iterator_types<_I>
        && !__detail::__cpp17_input_iterator<_I>
        && __detail::__cpp17_iterator<_I>)
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = typename __detail::__iterator_traits_difference_type<_I>::__type;
    using pointer               = void;
    using reference             = void;
};

template <class _T>
    requires is_object_v<_T>
struct iterator_traits<_T*>
{
    using iterator_concept      = contiguous_iterator_tag;
    using iterator_category     = random_access_iterator_tag;
    using value_type            = remove_cv_t<_T>;
    using difference_type       = ptrdiff_t;
    using pointer               = _T*;
    using reference             = _T&;
};


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_ITERATORTRAITS_H */
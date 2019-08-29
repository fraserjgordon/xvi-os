#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_INITIALIZERLIST_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_INITIALIZERLIST_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


// Note that initializer_list is magic and won't be used if this namespace isn't "std".
namespace __XVI_STD_LANGSUPPORT_NS
{


template <class _E> class initializer_list
{
public:

    using value_type        = _E;
    using reference         = const _E&;
    using const_reference   = const _E&;
    using size_type         = size_t;

    using iterator          = const _E*;
    using const_iterator    = const _E*;

    constexpr initializer_list() noexcept = default;

    constexpr size_t size() const noexcept
        { return _M_len; }

    constexpr const _E* begin() const noexcept
        { return _M_ptr; }

    constexpr const _E* end() const noexcept
        { return _M_ptr + _M_len; }

private:

    // Both GCC and Clang generate initializer_list objects as a pointer and length pair.
    iterator  _M_ptr = nullptr;
    size_type _M_len = 0;

    // Private constructor that the compiler calls.
    constexpr initializer_list(const_iterator __p, size_type __len)
        : _M_ptr(__p), _M_len(__len) {}
};

template <class _E> constexpr const _E* begin(initializer_list<_E> __il) noexcept
    { return __il.begin(); }
template <class _E> constexpr const _E* end(initializer_list<_E> __il) noexcept
    { return __il.end(); }


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_INITIALIZERLIST_H */

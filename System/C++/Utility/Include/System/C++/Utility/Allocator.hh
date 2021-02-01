#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ALLOCATOR_H
#define __SYSTEM_CXX_UTILITY_ALLOCATOR_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T>
class allocator
{
public:

    using value_type        = _T;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using is_always_equal   = true_type;
    using propagate_on_container_move_assignment = true_type;

    constexpr allocator() noexcept = default;
    constexpr allocator(const allocator&) noexcept = default;
    template <class _U> constexpr allocator(const allocator<_U>&) noexcept : allocator() {}

    constexpr ~allocator() = default;

    constexpr allocator& operator=(const allocator&) = default;

    [[nodiscard]] constexpr  _T* allocate(size_t __n)
    {
        if ((numeric_limits<size_t>::max() / sizeof(_T)) < __n)
            __XVI_CXX_UTILITY_THROW(bad_array_new_length());

        return static_cast<_T*>(::operator new(__n * sizeof(_T)));
    }

    constexpr void deallocate(_T* __p, size_t __n)
    {
        ::operator delete(__p, __n);
    }
};


template <class _T, class _U>
constexpr bool operator==(const allocator<_T>&, const allocator<_U>&) noexcept
{
    return true;
}

template <class _T, class _U>
constexpr bool operator!=(const allocator<_T>&, const allocator<_U>&) noexcept
{
    return false;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ALLOCATOR_H */

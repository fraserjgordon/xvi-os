#pragma once
#ifndef __SYSTEM_CXX_UTILITY_DEFAULTDELETE_H
#define __SYSTEM_CXX_UTILITY_DEFAULTDELETE_H


#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T>
struct default_delete
{
    constexpr default_delete() noexcept = default;

    template <class _U>
        requires std::convertible_to<_U*, _T*>
    default_delete(const default_delete<_U&>) noexcept
    {
    }

    void operator()(_T* __ptr) const
    {
        delete __ptr;
    }
};

template <class _T>
struct default_delete<_T[]>
{
    constexpr default_delete() noexcept = default;

    template <class _U>
        requires std::convertible_to<_U(*)[], _T(*)[]>
    default_delete(const default_delete<_U[]>&) noexcept
    {
    }

    template <class _U>
        requires std::convertible_to<_U(*)[], _T(*)[]>
    void operator()(_U* __ptr) const
    {
        delete[] __ptr;
    }
};


namespace __detail
{

template <class _T> using __default_delete_for
    = std::conditional_t<std::is_array_v<_T>, default_delete<std::remove_extent_t<_T>[]>, default_delete<_T>>; 

} // namespace __detail


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_DEFAULTDELETE_H */

#pragma once
#ifndef __SYSTEM_CXX_UTILITY_TYPEINDEX_H
#define __SYSTEM_CXX_UTILITY_TYPEINDEX_H


#include <System/C++/LanguageSupport/TypeInfo.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


class type_index
{
public:

    type_index(const type_info& __rhs) noexcept
        : _M_info(&__rhs)
    {
    }

    bool operator==(const type_index& __rhs) const noexcept
    {
        return *_M_info == *__rhs._M_info;
    }

    bool operator!=(const type_index& __rhs) const noexcept
    {
        return *_M_info != *__rhs._M_info;
    }

    bool operator< (const type_index& __rhs) const noexcept
    {
        return _M_info->before(*__rhs._M_info);
    }

    bool operator> (const type_index& __rhs) const noexcept
    {
        return __rhs._M_info->before(*_M_info);
    }

    bool operator<=(const type_index& __rhs) const noexcept
    {
        return !(__rhs._M_info->before(*_M_info));
    }

    bool operator>=(const type_index& __rhs) const noexcept
    {
        return !(_M_info->before(*__rhs._M_info));
    }

    size_t hash_code() const noexcept
    {
        return _M_info->hash_code();
    }

    const char* name() const noexcept
    {
        return _M_info->name();
    }

private:

    const type_info* _M_info;
};


template <class> struct hash;

template <> struct hash<type_index>
{
    size_t operator()(const type_index& __idx) const noexcept
    {
        return __idx.hash_code();
    }
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_TYPEINDEX_H */

#ifndef __SYSTEM_CXX_EXCEPTION_STDEXCEPT_H
#define __SYSTEM_CXX_EXCEPTION_STDEXCEPT_H


#include <System/C++/Core/StdExcept.hh>
#include <System/C++/String/CharTraits.hh>

#include <System/C++/Exception/Private/Config.hh>


namespace __XVI_STD_NS
{


namespace __detail
{


inline __exception_string::__ptr __exception_string::__make(const char* __str, size_t __strlen, size_t __len)
{
    auto __rawp = new byte[sizeof(__ptr) + __len + 1];
    auto __p = reinterpret_cast<__exception_string*>(__rawp);
    auto __strp = reinterpret_cast<char*>(__rawp + sizeof(__exception_string));

    new (__p) __exception_string();

    char_traits<char>::copy(__strp, __str, __strlen);
    __strp[__strlen] = '\0';

    return __ptr(__p);
}


template <class... _Args>
__exception_string::__ptr __make_exception_string(_Args&&... __args)
{
    auto __get_len = []<class _T>(const _T& __t)
    {
        if constexpr (is_same_v<_T, const char*> || is_same_v<_T, const char[]>)
            return char_traits<char>::length(__t);
        else if constexpr (is_array_v<_T>)
            return extent_v<_T>;
        else
            return __t.size();
    };

    auto __get_ptr = []<class _T>(const _T& __t)
    {
        if constexpr (is_same_v<_T, const char*> || is_array_v<_T>)
            return __t;
        else
            return __t.data();
    };

    size_t __len = (1 + ... + __get_len(__args));

    auto __ptr = __exception_string::__make("", 0, __len);

    (__builtin_strcat(__ptr.__raw(), __get_ptr(__args)), ...);

    return __ptr;
}


} // namespace __detail


} // namespace __XVI_STD_NS


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_STDEXCEPT_H */

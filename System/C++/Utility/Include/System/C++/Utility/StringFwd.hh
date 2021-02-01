#pragma once
#ifndef __SYSTEM_CXX_UTILITY_STRINGFWD_H
#define __SYSTEM_CXX_UTILITY_STRINGFWD_H


#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class> class allocator;
template <class> struct char_traits;


template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u8string = basic_string<char8_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;


template <class _CharT, class _Traits = char_traits<_CharT>> class basic_string_view;

using string_view    = basic_string_view<char>;
using u8string_view  = basic_string_view<char8_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using wstring_view   = basic_string_view<wchar_t>;



} // namespace __XVI_STD_STRING_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_STRINGFWD_H */

#pragma once
#ifndef __SYSTEM_CXX_UTILITY_IOSFWD_H
#define __SYSTEM_CXX_UTILITY_IOSFWD_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Utility/MBState.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class> struct char_traits;
template <> struct char_traits<char>;
template <> struct char_traits<char8_t>;
template <> struct char_traits<char16_t>;
template <> struct char_traits<char32_t>;
template <> struct char_traits<wchar_t>;

template <class> class allocator;


using streamoff = int64_t;
using streamsize = ptrdiff_t; 


} // namespace __XVI_STD_UTILITY_NS


namespace __XVI_STD_IOS_NS
{


using __XVI_STD_UTILITY_NS::char_traits;
using __XVI_STD_UTILITY_NS::allocator;


template <class _CharT, class = char_traits<_CharT>> class basic_ios;
template <class _CharT, class = char_traits<_CharT>> class basic_streambuf;
template <class _CharT, class = char_traits<_CharT>> class basic_istream;
template <class _CharT, class = char_traits<_CharT>> class basic_ostream;
template <class _CharT, class = char_traits<_CharT>> class basic_iostream;

template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_stringbuf;
template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_istringstream;
template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_ostringstream;
template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_stringstream;

template <class _CharT, class = char_traits<_CharT>> class basic_filebuf;
template <class _CharT, class = char_traits<_CharT>> class basic_ifstream;
template <class _CharT, class = char_traits<_CharT>> class basic_ofstream;
template <class _CharT, class = char_traits<_CharT>> class basic_fstream;

template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_syncbuf;
template <class _CharT, class = char_traits<_CharT>, class = allocator<_CharT>> class basic_osyncstream;

template <class _CharT, class = char_traits<_CharT>> class istreambuf_iterator;
template <class _CharT, class = char_traits<_CharT>> class ostreambuf_iterator;

using ios = basic_ios<char>;
using wios = basic_ios<wchar_t>;

using streambuf = basic_streambuf<char>;
using istream = basic_istream<char>;
using ostream = basic_ostream<char>;
using iostream = basic_iostream<char>;

using stringbuf = basic_stringbuf<char>;
using istringstream = basic_istringstream<char>;
using ostringstream = basic_ostringstream<char>;
using stringstream = basic_stringstream<char>;

using filebuf = basic_filebuf<char>;
using ifstream = basic_ifstream<char>;
using ofstream = basic_ofstream<char>;
using fstream = basic_fstream<char>;

using syncbuf = basic_syncbuf<char>;
using osyncstream = basic_osyncstream<char>;

using wstreambuf = basic_streambuf<wchar_t>;
using wistream = basic_istream<wchar_t>;
using wostream = basic_ostream<wchar_t>;
using wiostream = basic_iostream<wchar_t>;

using wstringbuf = basic_stringbuf<wchar_t>;
using wistringstream = basic_istringstream<wchar_t>;
using wostringstream = basic_ostringstream<wchar_t>;
using wstringstream = basic_stringstream<wchar_t>;

using wfilebuf = basic_filebuf<wchar_t>;
using wifstream = basic_ifstream<wchar_t>;
using wofstream = basic_ofstream<wchar_t>;
using wfstream = basic_fstream<wchar_t>;

using wsyncbuf = basic_syncbuf<wchar_t>;
using wosyncstream = basic_osyncstream<wchar_t>;

template <class> class fpos;
using streampos = fpos<mbstate_t>;
using wstreampos = fpos<mbstate_t>;
using u8streampos = fpos<mbstate_t>;
using u16streampos = fpos<mbstate_t>;
using u32streampos = fpos<mbstate_t>;


} // namespace __XVI_STD_IOS_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_IOSFWD_H */

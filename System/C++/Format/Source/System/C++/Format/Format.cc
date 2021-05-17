#undef __XVI_STD_FORMAT_HEADER_ONLY
#include <System/C++/Format/Format.hh>
#include <System/C++/Format/FormatImpl.hh>


namespace __XVI_STD_FORMAT_NS
{


namespace __detail
{


template class __library_formatter_base<char>;
template class __library_formatter_base<wchar_t>;

template class __library_formatter<char, char>;
    template format_context::iterator __library_formatter<char, char>::template format(char, format_context&);
template class __library_formatter<char, wchar_t>;
    template wformat_context::iterator __library_formatter<char, wchar_t>::template format(char, wformat_context&);
template class __library_formatter<wchar_t, wchar_t>;
    template wformat_context::iterator __library_formatter<wchar_t, wchar_t>::template format(wchar_t, wformat_context&);

template class __library_formatter<string_view, char>;
    template format_context::iterator __library_formatter<string_view, char>::template format(string_view, format_context&);
template class __library_formatter<wstring_view, wchar_t>;
    template wformat_context::iterator __library_formatter<wstring_view, wchar_t>::template format(wstring_view, wformat_context&);

template class __library_formatter<bool, char>;
    template format_context::iterator __library_formatter<bool, char>::template format(bool, format_context&);
template class __library_formatter<signed char, char>;
    template format_context::iterator __library_formatter<signed char, char>::template format(signed char, format_context&);
template class __library_formatter<unsigned char, char>;
    template format_context::iterator __library_formatter<unsigned char, char>::template format(unsigned char, format_context&);
template class __library_formatter<short, char>;
    template format_context::iterator __library_formatter<short, char>::template format(short, format_context&);
template class __library_formatter<unsigned short, char>;
    template format_context::iterator __library_formatter<unsigned short, char>::template format(unsigned short, format_context&);
template class __library_formatter<int, char>;
    template format_context::iterator __library_formatter<int, char>::template format(int, format_context&);
template class __library_formatter<unsigned int, char>;
    template format_context::iterator __library_formatter<unsigned int, char>::template format(unsigned int, format_context&);
template class __library_formatter<long, char>;
    template format_context::iterator __library_formatter<long, char>::template format(long, format_context&);
template class __library_formatter<unsigned long, char>;
    template format_context::iterator __library_formatter<unsigned long, char>::template format(unsigned long, format_context&);
template class __library_formatter<long long, char>;
    template format_context::iterator __library_formatter<long long, char>::template format(long long, format_context&);
template class __library_formatter<unsigned long long, char>;
    template format_context::iterator __library_formatter<unsigned long long, char>::template format(unsigned long long, format_context&);
template class __library_formatter<float, char>;
    template format_context::iterator __library_formatter<float, char>::template format(float, format_context&);
template class __library_formatter<double, char>;
    template format_context::iterator __library_formatter<double, char>::template format(double, format_context&);
template class __library_formatter<long double, char>;
    template format_context::iterator __library_formatter<long double, char>::template format(long double, format_context&);
template class __library_formatter<nullptr_t, char>;
    template format_context::iterator __library_formatter<nullptr_t, char>::template format(nullptr_t, format_context&);
template class __library_formatter<void*, char>;
    template format_context::iterator __library_formatter<void*, char>::template format(void*, format_context&);
template class __library_formatter<const void*, char>;
    template format_context::iterator __library_formatter<const void*, char>::template format(const void*, format_context&);

template class __library_formatter<bool, wchar_t>;
    template wformat_context::iterator __library_formatter<bool, wchar_t>::template format(bool, wformat_context&);
template class __library_formatter<signed char, wchar_t>;
    template wformat_context::iterator __library_formatter<signed char, wchar_t>::template format(signed char, wformat_context&);
template class __library_formatter<unsigned char, wchar_t>;
    template wformat_context::iterator __library_formatter<unsigned char, wchar_t>::template format(unsigned char, wformat_context&);
template class __library_formatter<short, wchar_t>;
    template wformat_context::iterator __library_formatter<short, wchar_t>::template format(short, wformat_context&);
template class __library_formatter<unsigned short, wchar_t>;
    template wformat_context::iterator __library_formatter<unsigned short, wchar_t>::template format(unsigned short, wformat_context&);
template class __library_formatter<int, wchar_t>;
    template wformat_context::iterator __library_formatter<int, wchar_t>::template format(int, wformat_context&);
template class __library_formatter<unsigned int, wchar_t>;
    template wformat_context::iterator __library_formatter<unsigned int, wchar_t>::template format(unsigned int, wformat_context&);
template class __library_formatter<long, wchar_t>;
    template wformat_context::iterator __library_formatter<long, wchar_t>::template format(long, wformat_context&);
template class __library_formatter<unsigned long, wchar_t>;
    template wformat_context::iterator __library_formatter<unsigned long, wchar_t>::template format(unsigned long, wformat_context&);
template class __library_formatter<long long, wchar_t>;
    template wformat_context::iterator __library_formatter<long long, wchar_t>::template format(long long, wformat_context&);
template class __library_formatter<unsigned long long, wchar_t>;
    template wformat_context::iterator __library_formatter<unsigned long long, wchar_t>::template format(unsigned long long, wformat_context&);
template class __library_formatter<float, wchar_t>;
    template wformat_context::iterator __library_formatter<float, wchar_t>::template format(float, wformat_context&);
template class __library_formatter<double, wchar_t>;
    template wformat_context::iterator __library_formatter<double, wchar_t>::template format(double, wformat_context&);
template class __library_formatter<long double, wchar_t>;
    template wformat_context::iterator __library_formatter<long double, wchar_t>::template format(long double, wformat_context&);
template class __library_formatter<nullptr_t, wchar_t>;
    template wformat_context::iterator __library_formatter<nullptr_t, wchar_t>::template format(nullptr_t, wformat_context&);
template class __library_formatter<void*, wchar_t>;
    template wformat_context::iterator __library_formatter<void*, wchar_t>::template format(void*, wformat_context&);
template class __library_formatter<const void*, wchar_t>;
    template wformat_context::iterator __library_formatter<const void*, wchar_t>::template format(const void*, wformat_context&);


template void __do_vformat(string_view, format_context&);
template void __do_vformat(wstring_view, wformat_context&);


} // namespace __detail


} // namespace __XVI_STD_FORMAT_NS

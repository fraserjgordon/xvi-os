#ifndef __SYSTEM_CXX_FORMAT_FORMAT_H
#define __SYSTEM_CXX_FORMAT_FORMAT_H


#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/CharTraits.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/String.hh>
#include <System/C++/Utility/StringView.hh>

#include <System/C++/Format/Private/Config.hh>


namespace __XVI_STD_FORMAT_NS
{


// Replace with proper locale support.
class locale;


namespace __detail
{

// Forward declarations.
//template <class _CharT> class __basic_fmt_contiguous_iter;
template <class _CharT> class __basic_fmt_n_iter;
template <class _CharT> class __basic_fmt_measuring_iter;
template <class _Context, class... _Args> class __format_arg_store;

}


//! @todo: REMOVE ME
namespace __detail
{
template <class _CharT> using __basic_fmt_contiguous_iter = std::back_insert_iterator<std::basic_string<_CharT>>;
}


// Forward declarations.
template <class _Out, class _CharT> class basic_format_context;
template <class _Context> class basic_format_args;
template <class _CharT, class... _Args> struct basic_format_string;

using format_context = basic_format_context<__detail::__basic_fmt_contiguous_iter<char>, char>;
using wformat_context = basic_format_context<__detail::__basic_fmt_contiguous_iter<wchar_t>, wchar_t>;

using format_args = basic_format_args<format_context>;
using wformat_args = basic_format_args<wformat_context>;

template <class... _Args> using format_string = basic_format_string<char, std::type_identity_t<_Args>...>;
template <class... _Args> using wformat_string = basic_format_string<wchar_t, std::type_identity_t<_Args>...>;

template <class _T, class _CharT = char> struct formatter;

template <class _CharT> class basic_format_parse_context;

using format_parse_context = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

template <class _Context> class basic_format_arg;

class format_error;

template <class _Visitor, class _Context> constexpr decltype(auto) visit_format_arg(_Visitor&& __vis, basic_format_arg<_Context> __arg);

template <class _Context = format_context, class... _Args> constexpr __detail::__format_arg_store<_Context, _Args...> make_format_args(_Args&&... __args);
template <class... _Args> constexpr __detail::__format_arg_store<wformat_context, _Args...> make_wformat_args(_Args&&... __args);

template <class... _Args> constexpr std::string format(format_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::wstring format(wformat_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::string format(const locale&, format_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::wstring format(const locale&, wformat_string<_Args...> __fmt, _Args&&... __args);

constexpr std::string vformat(std::string_view __fmt, format_args __args);
constexpr std::wstring vformat(std::wstring_view __fmt, wformat_args __args);
constexpr std::string vformat(const locale&, std::string_view __fmt, format_args __args);
constexpr std::wstring vformat(const locale&, std::wstring_view, wformat_args __args);

template <class _Out, class... _Args> constexpr _Out format_to(_Out __out, format_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr _Out format_to(_Out __out, wformat_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr _Out format_to(_Out __out, const locale&, format_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr _Out format_to(_Out __out, const locale&, wformat_string<_Args...> __fmt, _Args&&... ___args);

template <class _Out> constexpr _Out vformat_to(_Out __out, std::string_view __fmt, format_args __args);
template <class _Out> constexpr _Out vformat_to(_Out __out, std::wstring_view __fmt, wformat_args __args);
template <class _Out> constexpr _Out vformat_to(_Out __out, const locale&, std::string_view __fmt, format_args __args);
template <class _Out> constexpr _Out vformat_to(_Out __out, const locale&, std::wstring_view __fmt, wformat_args __args);

template <class _Out> struct format_to_n_result;

template <class _Out, class... _Args> constexpr format_to_n_result<_Out> format_to_n(_Out __out, std::iter_difference_t<_Out> __n, format_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr format_to_n_result<_Out> format_to_n(_Out __out, std::iter_difference_t<_Out> __n, wformat_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr format_to_n_result<_Out> format_to_n(_Out __out, std::iter_difference_t<_Out> __n, const locale&, format_string<_Args...> __fmt, _Args&&... __args);
template <class _Out, class... _Args> constexpr format_to_n_result<_Out> format_to_n(_Out __out, std::iter_difference_t<_Out> __n, const locale&, wformat_string<_Args...> __fmt, _Args&&... __args);

template <class... _Args> constexpr std::size_t formatted_size(format_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::size_t formatted_size(wformat_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::size_t formatted_size(const locale&, format_string<_Args...> __fmt, _Args&&... __args);
template <class... _Args> constexpr std::size_t formatted_size(const locale&, wformat_string<_Args...> __fmt, _Args&&... __args);


class format_error :
    public std::runtime_error
{
public:

    explicit format_error(const std::string& __what) :
        std::runtime_error(__what)
    {
    }

    explicit format_error(const char* __what) :
        std::runtime_error(__what)
    {
    }
};


namespace __detail
{


// Scans a format string for the next replacement field.
template <class _CharT>
constexpr std::basic_string_view<_CharT> __find_next_replacement_field(const std::basic_string_view<_CharT> __s)
{
    auto __p = __s.begin();
    while (__p != __s.end())
    {
        if (*__p != static_cast<_CharT>('{') && *__p != static_cast<_CharT>('}'))
        {
            // Not the start of a replacement field.
            ++__p;
            continue;
        }

        // Sanity check: we can read at least one more character.
        auto __next = std::next(__p);
        if (__next == __s.end()) [[unlikely]]
            throw format_error("invalid format string: unterminated replacement field");

        // Found the beginning of a replacement field. Check for special cases.
        if (*__p == *__next)
        {
            // This is a "{{" or "}}" sequence.
            return {__p, 2};
        }
        else if (*__p == static_cast<_CharT>('}'))
        {
            // Unbalanced isolated '}' chars are not permitted in format strings.
            throw format_error("invalid format string: unexpected '}' character");
        }

        // Scan for the end of the replacement field. Because "{...}" can be nested, we need to count the depth.
        std::size_t __depth = 1;
        for (; __next != __s.end(); ++__next)
        {
            if (*__next == static_cast<_CharT>('{'))
            {
                ++__depth;
            }
            else if (*__next == static_cast<_CharT>('}'))
            {
                if (__depth == 1)
                {
                    // Last matching '}' has been found.
                    return {__p, static_cast<std::size_t>(std::next(__next) - __p)};
                }

                --__depth;
            }
        }

        // End of format string reached but replacement field was unterimated.
        throw format_error("invalid format string: unterminated replacement field");
    }

    // No more replacements found. Return an empty one at the end of the input string.
    return __s.substr(__s.size(), 0);
}


// Replacement field argument ID type.
enum class __replacement_arg_id_type
{
    __unknown,
    __explicit,
    __implicit,
};

// Type-independent representation of a replacement field.
template <class _CharT>
struct __replacement_field_info
{
    std::basic_string_view<_CharT>      __format_spec = {};
    std::size_t                         __arg_id = 0;
    __replacement_arg_id_type           __arg_id_type = __replacement_arg_id_type::__unknown;
    _CharT                              __replacement_char = 0;
};


template <class _CharT>
constexpr bool __is_digit(_CharT __c)
{
    return (static_cast<_CharT>('0') <= __c && __c <= static_cast<_CharT>('9'));
}

template <class _CharT>
constexpr bool __is_nonzero_digit(_CharT __c)
{
    return __is_digit(__c) && __c != static_cast<_CharT>('0');
}

template <class _CharT>
constexpr std::size_t __extract_int(std::basic_string_view<_CharT>& __s)
{
    // Note that leading zeroes are never permitted.
    std::size_t __val = 0;
    if (__s.size() >= 2 && __s[0] == static_cast<_CharT>('0'))
        throw format_error("invalid format string: leading zeroes are forbidden in replacement fields");

    while (!__s.empty() && __is_digit(__s[0]))
    {
        __val = (10 * __val) + static_cast<std::size_t>(__s[0] - static_cast<_CharT>('0'));
        __s.remove_prefix(1);
    }

    return __val;
}

// Parses a replacement field into its constituent components.
template <class _CharT>
constexpr __replacement_field_info<_CharT> __parse_replacement_field(std::basic_string_view<_CharT> __s)
{
    // Check for the special case of a "{{" or "}}" replacement.
    if (__s[0] == __s[1])
        return {{}, 0, __replacement_arg_id_type::__unknown, __s[0]};

    // Begin parsing the field. Until we know otherwise, we assume an implicit argument numbering.
    auto __p = std::next(__s.begin());
    __replacement_field_info<_CharT> __info;
    __info.__arg_id_type = __replacement_arg_id_type::__implicit;

    // An initial sequence of digits gives us an explicit argument id.
    if (auto __c = *__p; __is_digit(__c))
    {
        __info.__arg_id_type = __replacement_arg_id_type::__explicit;

        // The format string spec explicitly disallows leading zeroes.
        if (__c == static_cast<_CharT>('0'))
        {
            if (__is_digit(*++__p))
                throw format_error("invalid format string: argument IDs must not have leading zeroes");
            else
                __info.__arg_id = 0;
        }
        else
        {
            // Read and calculate the argument ID.
            while (__is_digit(*__p))
                __info.__arg_id = (10 * __info.__arg_id) + static_cast<std::size_t>(*__p++ - static_cast<_CharT>('0'));
        }
    }

    // Now that the argument ID (if present) has been parsed, check for a format specifier string.
    if (*__p == static_cast<_CharT>(':'))
    {
        // The rest of the string is the format specifier.
        __info.__format_spec = {__p + 1, static_cast<std::size_t>(__s.end() - (__p + 1) - 1)};
        __p = __s.end() - 1;
    }

    // No other contents (except the replacement field terminator) are permitted.
    if (__p != __s.end() - 1)
        throw format_error("invalid format string: malformed replacement field");

    return __info;
}


class __disabled_formatter
{
public:

    __disabled_formatter() = delete;
    __disabled_formatter(const __disabled_formatter&) = delete;
    __disabled_formatter(__disabled_formatter&&) = delete;

    void operator=(const __disabled_formatter&) = delete;
    void operator=(__disabled_formatter&&) = delete;
};


template <class _T, class _Context>
concept __is_formattable = requires (_T& __t, _Context& __ctxt)
{
    (typename _Context::template formatter_type<_T>()).format(__t, __ctxt);
};


template <class _T, class _Context>
concept __is_const_formattable = requires (const _T& __t, _Context& __ctxt)
{
    (typename _Context::template formatter_type<_T>()).format(__t, __ctxt);
};


// Type of argument stored in a basic_format_arg object.
enum class __fmt_arg_type
{
    __none,
    __bool,
    __char,
    __int,
    __unsigned_int,
    __long_long_int,
    __unsigned_long_long_int,
    __float,
    __double,
    __long_double,
    __cstring,
    __string_view,
    __void_ptr,
    __handle,
};


template <class _CharT>
using __basic_format_handle_lambda = void (*)(basic_format_parse_context<_CharT>& __pc, void* __fc, const void* __obj);


template <class _CharT>
struct __basic_format_arg_handle
{
    void*                                   __data;
    __basic_format_handle_lambda<_CharT>    __fmt_fn;
};


template <class _CharT>
union __basic_format_arg_storage
{
    bool                                _U_bool;
    _CharT                              _U_char;
    int                                 _U_int;
    unsigned int                        _U_unsigned_int;
    long long                           _U_long_long_int;
    unsigned long long                  _U_unsigned_long_long_int;
    float                               _U_float;
    double                              _U_double;
    long double                         _U_long_double;
    const _CharT*                       _U_cstring;
    std::basic_string_view<_CharT>      _U_string_view = {};
    const void*                         _U_void_ptr;
    __basic_format_arg_handle<_CharT>   _U_handle;
};


template <class _CharT, class _Visitor>
constexpr decltype(auto) __visit_format_arg(__fmt_arg_type __type, const __basic_format_arg_storage<_CharT>& __data, _Visitor&& __visitor)
{
    switch (__type)
    {
        case __fmt_arg_type::__bool:
            return std::forward<_Visitor>(__visitor)(__data._U_bool);

        case __fmt_arg_type::__char:
            return std::forward<_Visitor>(__visitor)(__data._U_char);

        case __fmt_arg_type::__int:
            return std::forward<_Visitor>(__visitor)(__data._U_int);

        case __fmt_arg_type::__unsigned_int:
            return std::forward<_Visitor>(__visitor)(__data._U_unsigned_int);

        case __fmt_arg_type::__long_long_int:
            return std::forward<_Visitor>(__visitor)(__data._U_long_long_int);

        case __fmt_arg_type::__unsigned_long_long_int:
            return std::forward<_Visitor>(__visitor)(__data._U_unsigned_long_long_int);

        case __fmt_arg_type::__float:
            return std::forward<_Visitor>(__visitor)(__data._U_float);

        case __fmt_arg_type::__double:
            return std::forward<_Visitor>(__visitor)(__data._U_double);

        case __fmt_arg_type::__long_double:
            return std::forward<_Visitor>(__visitor)(__data._U_long_double);

        case __fmt_arg_type::__cstring:
            return std::forward<_Visitor>(__visitor)(__data._U_cstring);

        case __fmt_arg_type::__string_view:
            return std::forward<_Visitor>(__visitor)(__data._U_string_view);

        case __fmt_arg_type::__void_ptr:
            return std::forward<_Visitor>(__visitor)(__data._U_void_ptr);

        case __fmt_arg_type::__handle:
            return std::forward<_Visitor>(__visitor)(__data._U_handle);

        case __fmt_arg_type::__none:
            throw format_error("visiting empty format arg");
    }

    throw format_error("internal error: this statement should be unreachable");
}


// Templated struct allowing compile-time detection of the storage type used by __store_format_arg.
template <__fmt_arg_type _Type> struct __fmt_arg_type_tag {};


template <class _Context, class _Char = typename _Context::char_type>
struct __store_format_arg_helper
{
    __fmt_arg_type& __type;
    __basic_format_arg_storage<_Char>& __data;

    template <class _Tr>
        requires __is_formattable<std::remove_cvref_t<_Tr>, _Context>
    constexpr auto __store_format_arg(_Tr&& __v) noexcept
    {
        using _T = std::remove_cvref_t<_Tr>;

        if constexpr (std::is_same_v<_T, bool>)
        {
            __type = __fmt_arg_type::__bool;
            __data._U_bool = __v;
            return __fmt_arg_type_tag<__fmt_arg_type::__bool>{};
        }
        else if constexpr (std::is_same_v<_T, _Char>)
        {
            __type = __fmt_arg_type::__char,
            __data._U_char = __v;
            return __fmt_arg_type_tag<__fmt_arg_type::__char>{};
        }
        else if constexpr (std::is_same_v<_T, char> && std::is_same_v<_Char, wchar_t>)
        {
            __type = __fmt_arg_type::__char,
            __data._U_char = static_cast<wchar_t>(__v);
            return __fmt_arg_type_tag<__fmt_arg_type::__char>{};
        }
        else if constexpr (std::is_integral_v<_T> && std::is_signed_v<_T> && sizeof(_T) <= sizeof(int))
        {
            __type = __fmt_arg_type::__int;
            __data._U_int = static_cast<int>(__v);
            return __fmt_arg_type_tag<__fmt_arg_type::__int>{};
        }
        else if constexpr (std::is_integral_v<_T> && std::is_unsigned_v<_T> && sizeof(_T) <= sizeof(unsigned int))
        {
            __type = __fmt_arg_type::__unsigned_int;
            __data._U_unsigned_int = static_cast<unsigned int>(__v);
            return __fmt_arg_type_tag<__fmt_arg_type::__unsigned_int>{};
        }
        else if constexpr (std::is_integral_v<_T> && std::is_signed_v<_T> && sizeof(_T) <= sizeof(long long int))
        {
            __type = __fmt_arg_type::__long_long_int;
            __data._U_long_long_int = static_cast<long long int>(__v);
            return __fmt_arg_type_tag<__fmt_arg_type::__long_long_int>{};
        }
        else if constexpr (std::is_integral_v<_T> && std::is_unsigned_v<_T> && sizeof(_T) <= sizeof(unsigned long long))
        {
            __type = __fmt_arg_type::__unsigned_long_long_int;
            __data._U_unsigned_long_long_int = static_cast<unsigned long long int>(__v);
            return __fmt_arg_type_tag<__fmt_arg_type::__unsigned_long_long_int>{};
        }
        else
        {
            // Stored as a handle.
            __type = __fmt_arg_type::__handle;

            // Generate a lambda to handle the formatting.
            auto __format_lambda = [](basic_format_parse_context<_Char>& __parse, void* /*_Context&*/ __format, const void* /*const _T&*/ __ptr) constexpr
            {
                using _TD = std::remove_cvref_t<_T>;
                using _TQ = std::conditional_t<__is_const_formattable<_TD, _Context>, const _TD, _TD>;

                // If called without an object, we're only parsing the format spec, not actually formatting anything.
                bool __parse_only = !__ptr;

                typename _Context::template formatter_type<_TD> __f;

                // We can parse as a constexpr (no casts of void pointers required).
                __parse.advance_to(__f.parse(__parse));

                // But we can't format as constexpr (void* casts are not allowed).
                if (!__parse_only)
                {
                    auto& __fc = *reinterpret_cast<_Context*>(__format);
                    __fc.advance_to(__f.format(*const_cast<_TQ*>(static_cast<const _TD*>(__ptr)), __fc));
                }
            };

            __data._U_handle =
            {
                .__data = std::addressof(__v),
                .__fmt_fn = __format_lambda,
            };

            return __fmt_arg_type_tag<__fmt_arg_type::__handle>{};
        }
    }

    constexpr auto __store_format_arg(float __f)
    {
        __type = __fmt_arg_type::__float;
        __data._U_float = __f;
        return __fmt_arg_type_tag<__fmt_arg_type::__float>{};
    }

    constexpr auto __store_format_arg(double __f) noexcept
    {
        __type = __fmt_arg_type::__double;
        __data._U_double = __f;
        return __fmt_arg_type_tag<__fmt_arg_type::__double>{};
    }

    constexpr auto __store_format_arg(long double __f) noexcept
    {
        __type = __fmt_arg_type::__long_double;
        __data._U_long_double = __f;
        return __fmt_arg_type_tag<__fmt_arg_type::__long_double>{};
    }

    constexpr auto __store_format_arg(const _Char* __s) noexcept
    {
        __type = __fmt_arg_type::__cstring;
        __data._U_cstring = __s;
        return __fmt_arg_type_tag<__fmt_arg_type::__cstring>{};
    }

    template <class _Traits>
    constexpr auto __store_format_arg(std::basic_string_view<_Char, _Traits> __s) noexcept
    {
        __type = __fmt_arg_type::__string_view;
        __data._U_string_view = {__s.data(), __s.size()};
        return __fmt_arg_type_tag<__fmt_arg_type::__string_view>{};
    }

    template <class _Traits, class _Allocator>
    constexpr auto __store_format_arg(const std::basic_string<_Char, _Traits, _Allocator>& __s) noexcept
    {
        __type = __fmt_arg_type::__string_view;
        __data._U_string_view = {__s.data(), __s.size()};
        return __fmt_arg_type_tag<__fmt_arg_type::__string_view>{};
    }

    constexpr auto __store_format_arg(std::nullptr_t) noexcept
    {
        __type = __fmt_arg_type::__void_ptr;
        __data._U_void_ptr = nullptr;
        return __fmt_arg_type_tag<__fmt_arg_type::__void_ptr>{};
    }

    template <class _T>
        requires std::is_void_v<_T>
    constexpr auto __store_format_arg(_T* __p) noexcept
    {
        __type = __fmt_arg_type::__void_ptr;
        __data._U_void_ptr = __p;
        return __fmt_arg_type_tag<__fmt_arg_type::__void_ptr>{};
    }
};


template <class _Context, class _T, class _CharT = typename _Context::char_type>
constexpr auto __store_format_arg(_T&& __t, __fmt_arg_type& __type, __basic_format_arg_storage<_CharT>& __data)
{
    // Perform some type calculus to make sure the correct overload is used:
    //
    //  - step 1: strip all references for fundamental or pointer types.
    //  - step 2: add const to pointers, if we can.
    //  - step 3: add const to references, if we can.
    using _T0 = std::remove_cvref_t<_T>;
    using _T1 = std::conditional_t<std::is_fundamental_v<_T0> || std::is_pointer_v<_T0> || std::is_member_pointer_v<_T0>, _T0, _T>;
    using _T2 = std::conditional_t<std::is_pointer_v<_T1> && __is_formattable<std::add_pointer_t<const std::remove_pointer_t<_T1>>, _Context>, std::add_pointer_t<const std::remove_pointer_t<_T1>>, _T1>;
    using _T3 = std::conditional_t<std::is_reference_v<_T2> && __is_const_formattable<std::remove_cvref_t<_T2>, _Context>, const std::remove_cvref_t<_T2>&, _T2>;

    if constexpr (std::is_reference_v<_T3>)
        return __store_format_arg_helper<_Context, _CharT>{__type, __data}.__store_format_arg(std::forward<_T3>(__t));
    else
        return __store_format_arg_helper<_Context, _CharT>{__type, __data}.__store_format_arg(static_cast<_T3>(__t));
}


template <class _Context, class... _Args>
class __format_arg_store
{
public:

    std::array<__fmt_arg_type, sizeof...(_Args)>                                            _M_types;
    std::array<__basic_format_arg_storage<typename _Context::char_type>, sizeof...(_Args)>  _M_args;
};


template <class _Context, std::size_t... _Idx, class... _Args>
constexpr __format_arg_store<_Context, _Args...> __make_format_args(std::index_sequence<_Idx...>, _Args&&... __args)
{
    __format_arg_store<_Context, _Args...> __store;

    (__store_format_arg<_Context>(std::forward<_Args>(__args), __store._M_types[_Idx], __store._M_args[_Idx]), ...);

    return __store;
}


} // namespace __detail


template <class _CharT, class... Args>
class basic_format_string
{
public:

    template <class _T>
        requires std::convertible_to<const _T&, std::basic_string_view<_CharT>>
    consteval basic_format_string(const _T& __s);

    constexpr basic_string_view<_CharT> get() const noexcept
    {
        return _M_str;
    }

private:

    std::basic_string_view<_CharT>      _M_str;
};


template <class _CharT>
class basic_format_parse_context
{
public:

    using char_type         = _CharT;
    using const_iterator    = typename std::basic_string_view<_CharT>::const_iterator;
    using iterator          = const_iterator;

    constexpr explicit basic_format_parse_context(std::basic_string_view<_CharT> __fmt, std::size_t __num_args = 0) noexcept :
        _M_fmt{__fmt},
        _M_arg_count{__num_args}
    {
    }

    basic_format_parse_context(const basic_format_parse_context&) = delete;
    basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

    constexpr const_iterator begin() const noexcept
    {
        return _M_fmt.begin();
    }

    constexpr const_iterator end() const noexcept
    {
        return _M_fmt.end();
    }

    constexpr void advance_to(const_iterator __it)
    {
        _M_fmt = {__it, static_cast<std::size_t>(end() - __it)};
    }

    constexpr void __set(std::basic_string_view<_CharT> __sv)
    {
        _M_fmt = __sv;
    }

    constexpr std::size_t next_arg_id()
    {
        if (_M_index_type == __detail::__replacement_arg_id_type::__explicit)
            throw format_error("invalid format string: mixed implicit and explicit argument numbers");

        _M_index_type = __detail::__replacement_arg_id_type::__implicit;

        return _M_next_arg_id++;
    }

    constexpr void check_arg_id(std::size_t __id)
    {
        if (_M_index_type == __detail::__replacement_arg_id_type::__implicit)
            throw format_error("invalid format string: mixed implicit and explicit argument numbers");

        _M_index_type = __detail::__replacement_arg_id_type::__explicit;

        // This function is not a constant expression if the argument ID is invalid. Throwing is sufficient to do that.
        if (std::is_constant_evaluated() && __id >= _M_arg_count)
            throw format_error("invalid format string: references invalid argument number");
    }

private:

    std::basic_string_view<_CharT>      _M_fmt = {};
    __detail::__replacement_arg_id_type _M_index_type = __detail::__replacement_arg_id_type::__unknown;
    std::size_t                         _M_next_arg_id = 0;
    std::size_t                         _M_arg_count = 0;
};



template <class _Context>
class basic_format_arg
{
    friend class basic_format_args<_Context>;

public:

    using char_type = typename _Context::char_type;

    using __fmt_arg_type = __detail::__fmt_arg_type;

    class handle
    {
        friend class basic_format_arg<_Context>;

    public:

        constexpr void __parse(basic_format_parse_context<char_type>& __pc) const
        {
            _M_fmt_fn(__pc, nullptr, nullptr);
        }

        // Not constexpr as the type erasure is incompatible with constexpr evaluation.
        void format(basic_format_parse_context<char_type>& __pc, _Context& __context) const
        {
            _M_fmt_fn(__pc, &__context, _M_ptr);
        }

    private:

        const void*                                         _M_ptr = nullptr;
        __detail::__basic_format_handle_lambda<char_type>   _M_fmt_fn = nullptr;

        constexpr handle(const void* __ptr, __detail::__basic_format_handle_lambda<char_type> __fmt) :
            _M_ptr{__ptr},
            _M_fmt_fn{__fmt}
        {
        }
    };

    constexpr basic_format_arg() noexcept = default;

    constexpr explicit operator bool() const noexcept
    {
        return _M_type != __fmt_arg_type::__none;
    }


    template <class _Visitor>
    constexpr decltype(auto) __visit(_Visitor&& __visitor)
    {
        return __detail::__visit_format_arg(_M_type, *_M_data, [__visitor = std::forward<_Visitor>(__visitor)]<class _Arg>(_Arg&& __arg) constexpr mutable
        {
            if constexpr (std::is_same_v<std::remove_cvref_t<_Arg>, __detail::__basic_format_arg_handle<char_type>>)
                return std::forward<_Visitor>(__visitor)(handle{__arg.__data, __arg.__fmt_fn});
            else
                return std::forward<_Visitor>(__visitor)(std::forward<_Arg>(__arg));
        });
    }

private:

    __detail::__fmt_arg_type                                _M_type = __fmt_arg_type::__none;
    const __detail::__basic_format_arg_storage<char_type>*  _M_data = {};


    constexpr basic_format_arg(__detail::__fmt_arg_type __type, const __detail::__basic_format_arg_storage<char_type>* __data) :
        _M_type{__type},
        _M_data{__data}
    {
    }
};


template <class _Out, class _CharT>
class basic_format_args<basic_format_context<_Out, _CharT>>
{
public:

    using _Context = basic_format_context<_Out, _CharT>;

    constexpr basic_format_args() noexcept = default;

    template <class... _Args>
    constexpr basic_format_args(const __detail::__format_arg_store<_Context, _Args...>& __args) noexcept :
        _M_size{sizeof...(_Args)},
        _M_types{__args._M_types.data()},
        _M_data{__args._M_args.data()}
    {
    }

    constexpr basic_format_arg<_Context> get(std::size_t __i) const noexcept
    {
        if (__i >= _M_size)
            return {};

        return {_M_types[__i], std::addressof(_M_data[__i])};   
    }

    constexpr std::size_t __size() const noexcept
    {
        return _M_size;
    }

private:

    std::size_t                                         _M_size = 0;
    const __detail::__fmt_arg_type*                     _M_types = nullptr;
    const __detail::__basic_format_arg_storage<_CharT>* _M_data = nullptr;
};


template <class _Visitor, class _Context>
constexpr decltype(auto) visit_format_arg(_Visitor&& __visitor, basic_format_arg<_Context> __context)
{
    return __context.__visit(std::forward<_Visitor>(__visitor));
}


template <class _Context, class... _Args>
constexpr __detail::__format_arg_store<_Context, _Args...> make_format_args(_Args&&... __args)
{
    return __detail::__make_format_args<_Context>(std::make_index_sequence<sizeof...(_Args)>(), std::forward<_Args>(__args)...);
}


template <class _Out, class _CharT>
class basic_format_context
{
public:

    using char_type     = _CharT;
    using iterator      = _Out;

    template <class _T> using formatter_type = formatter<_T, _CharT>;

    constexpr basic_format_arg<basic_format_context> arg(std::size_t __id) const noexcept
    {
        return _M_args.get(__id);
    }

    std::locale locale();

    constexpr iterator out()
    {
        return std::move(_M_out);
    }

    constexpr void advance_to(iterator __it)
    {
        _M_out = std::move(__it);
    }

    static constexpr basic_format_context __make(basic_format_args<basic_format_context> __args, _Out __out)
    {
        return basic_format_context{std::move(__args), std::move(__out)};
    }

private:

    basic_format_args<basic_format_context>     _M_args;
    _Out                                        _M_out;


    constexpr basic_format_context(basic_format_args<basic_format_context> __args, _Out __out) :
        _M_args{std::move(__args)},
        _M_out{std::move(__out)}
    {
    }
};


namespace __detail
{


// Alignment types for stdlib formatters.
enum class __stdfmt_align_type
{
    __default = 0,
    __begin = '<',
    __middle = '^',
    __end = '>',
};

// Sign display types for stdlib formatters.
enum class __stdfmt_sign_type
{
    __implicit_default,
    __default = '-',
    __always = '+',
    __leading_space = ' ',  // Leading space for non-negative numbers, to maintain alignment.
};

// Ways that the width/precision can be specified for stdlib formatters.
enum class __stdfmt_wp_type
{
    __none,
    __fixed,
    __runtime_arg_implicit,
    __runtime_arg_explicit,
};

// Presentation types for stdlib formatters.
enum class __stdfmt_presentation_type : unsigned char
{
    __default = 0,
    __floating_point,   // Default floating-point presentation.
    __a = 'a',
    __A = 'A',
    __b = 'b',
    __B = 'B',
    __c = 'c',
    __d = 'd',
    __e = 'e',
    __f = 'f',
    __F = 'F',
    __g = 'g',
    __G = 'G',
    __o = 'o',
    __p = 'p',
    __s = 's',
    __x = 'x',
    __X = 'X',
};

// Parsed format spec for stdlib formatters.
template <class _CharT>
struct __stdfmt_format_spec
{
    bool                    __alt_form = false;
    bool                    __zero_pad = false;
    bool                    __locale_specific = false;
    bool                    __implicit_alignment = false;

    __stdfmt_align_type     __align = __stdfmt_align_type::__default;
    __stdfmt_sign_type      __sign = __stdfmt_sign_type::__implicit_default;
    __stdfmt_wp_type        __width = __stdfmt_wp_type::__none;
    __stdfmt_wp_type        __precision = __stdfmt_wp_type::__none;
    
    _CharT                  __fill_char = static_cast<_CharT>(' ');
    std::size_t             __width_val = 0;
    std::size_t             __precision_val = 0;

    __stdfmt_presentation_type __type = __stdfmt_presentation_type::__default;
};


// Attempts to parse the given char as an alignment char.
template <class _CharT>
constexpr bool __parse_stdfmt_align(_CharT __c, __stdfmt_format_spec<_CharT>& __spec)
{
    switch (__c)
    {
        case static_cast<_CharT>('<'):
            __spec.__align = __stdfmt_align_type::__begin;
            return true;

        case static_cast<_CharT>('^'):
            __spec.__align = __stdfmt_align_type::__middle;
            return true;

        case static_cast<_CharT>('>'):
            __spec.__align = __stdfmt_align_type::__end;
            return true;

        default:
            return false;
    }
}

// Attempts to parse the given char as a sign char.
template <class _CharT>
constexpr bool __parse_stdfmt_sign(_CharT __c, __stdfmt_format_spec<_CharT>& __spec)
{
    switch (__c)
    {
        case static_cast<_CharT>('+'):
            __spec.__sign = __stdfmt_sign_type::__always;
            return true;

        case static_cast<_CharT>('-'):
            __spec.__sign = __stdfmt_sign_type::__default;
            return true;

        case static_cast<_CharT>(' '):
            __spec.__sign = __stdfmt_sign_type::__leading_space;
            return true;

        default:
            return false;
    }
}

// Parses a format spec for a stdlib formatter.
template <class _CharT>
constexpr __stdfmt_format_spec<_CharT> __parse_stdfmt_format_spec(std::basic_string_view<_CharT> __s)
{
    __stdfmt_format_spec<_CharT> __spec = {};

    // Short-circuit check for the common case of an empty format spec.
    if (__s.empty())
        return {};

    // The fill character can be anything other than '{' or '}'. Because of this, we need to look ahead for a following
    // alignment char and only if that alignment char is present will the first char be a fill char.
    if (__s.size() >= 2)
    {
        // We don't explicitly check for and disallow '{' or '}' here. If they were present, they would have caused a
        // mis-parse of the formatting string at an earlier stage due to unbalanced parentheses in the format string.

        // Try parsing the second char as an alignment type.
        if (__parse_stdfmt_align(__s[1], __spec))
        {
            __spec.__fill_char = __s[0];
            __s.remove_prefix(2);
        }
    }

    // If we didn't find a fill+alignment pair already, check again for an alignment.
    if (__spec.__align == __stdfmt_align_type::__default)
    {
        if (!__s.empty() && __parse_stdfmt_align(__s[0], __spec))
            __s.remove_prefix(1);
    }

    // Check for a sign specifier.
    if (!__s.empty() && __parse_stdfmt_sign(__s[0], __spec))
        __s.remove_prefix(1);

    // Check for an alternative-form flag.
    if (!__s.empty() && __s[0] == static_cast<_CharT>('#'))
    {
        __spec.__alt_form = true;
        __s.remove_prefix(1);
    }

    // Check for a zero-pad flag.
    if (!__s.empty() && __s[0] == static_cast<_CharT>('0'))
    {
        __spec.__zero_pad = true;
        __s.remove_prefix(1);
    }

    // Check for a width field.
    if (!__s.empty())
    {
        if (__is_digit(__s[0]))
        {
            // Fixed width.
            __spec.__width = __stdfmt_wp_type::__fixed;
            __spec.__width_val = __extract_int(__s);
        }
        else if (__s[0] == static_cast<_CharT>('{'))
        {
            // Runtime width. Check for implicit or explicit argument numbering.
            //
            // Note: we can safely assume there is at least one more character as the balancing of the parentheses was
            //       confirmed during the initial parsing of the format string.
            __s.remove_prefix(1);
            __spec.__width = __stdfmt_wp_type::__runtime_arg_implicit;
            if (__is_digit(__s[0]))
            {
                __spec.__width = __stdfmt_wp_type::__runtime_arg_explicit;
                __spec.__width_val = __extract_int(__s);
            }

            // We do still need to check we're at the closing parenthesis so we can reject invalid runtime width specs.
            if (__s[0] != static_cast<_CharT>('}'))
                throw format_error("invalid format string: malformed width arg in format specifier");

            __s.remove_prefix(1);
        }
    }

    // Check for a precision field.
    if (!__s.empty() && __s[0] == static_cast<_CharT>('.'))
    {
        // Empty precisions are not permitted.
        __s.remove_prefix(1);
        if (__s.empty())
            throw format_error("invalid format string: empty precision in format specifier");

        if (__is_digit(__s[0]))
        {
            // Fixed precision.
            __spec.__precision = __stdfmt_wp_type::__fixed;
            __spec.__precision_val = __extract_int(__s);
        }
        else if (__s[0] == static_cast<_CharT>('{'))
        {
            // Runtime precision. Check for implicit or explicit argument numbering.
            //
            // Note: we can safely assume there is at least one more character as the balancing of the parentheses was
            //       confirmed during the initial parsing of the format string.
            __s.remove_prefix(1);
            __spec.__precision = __stdfmt_wp_type::__runtime_arg_implicit;
            if (__is_digit(__s[0]))
            {
                __spec.__precision = __stdfmt_wp_type::__runtime_arg_explicit;
                __spec.__precision_val = __extract_int(__s);
            }

            // We do still need to check we're at the closing parenthesis so we can reject invalid runtime width specs.
            if (__s[0] != static_cast<_CharT>('}'))
                throw format_error("invalid format string: malformed precision arg in format specifier");

            __s.remove_prefix(1);
        }
    }

    // Check for a locale-specific flag.
    if (!__s.empty() && __s[0] == static_cast<_CharT>('L'))
    {
        __spec.__locale_specific = true;
        __s.remove_prefix(1);
    }

    // Finally, extract the presentation type.
    if (!__s.empty())
    {
        switch (__s[0])
        {
            case static_cast<_CharT>('a'):
            case static_cast<_CharT>('A'):
            case static_cast<_CharT>('b'):
            case static_cast<_CharT>('B'):
            case static_cast<_CharT>('c'):
            case static_cast<_CharT>('d'):
            case static_cast<_CharT>('e'):
            case static_cast<_CharT>('E'):
            case static_cast<_CharT>('f'):
            case static_cast<_CharT>('F'):
            case static_cast<_CharT>('g'):
            case static_cast<_CharT>('G'):
            case static_cast<_CharT>('o'):
            case static_cast<_CharT>('p'):
            case static_cast<_CharT>('s'):
            case static_cast<_CharT>('x'):
            case static_cast<_CharT>('X'):
                __spec.__type = static_cast<__stdfmt_presentation_type>(__s[0]);
                __s.remove_prefix(1);
                break;

            default:
                throw format_error("invalid format string: unknown presentation type");
        }
    }

    // We must now be at the end of the format spec.
    if (!__s.empty())
        throw format_error("invalid format string: extra character(s) at end of format specifier");

    return __spec;
}


template <class _CharT>
struct __stdfmt_formatter_base
{
    using _PC = basic_format_parse_context<_CharT>;
    template <class _Out> using _FC = basic_format_context<_Out, _CharT>;

    __stdfmt_format_spec<_CharT> _M_spec = {};

    constexpr typename _PC::iterator parse(_PC& __pc)
    {
        // Do a type-generic parse of the format specification.
        std::basic_string_view<_CharT> __sv{__pc.begin(), static_cast<std::size_t>(__pc.end() - __pc.begin())};
        _M_spec = __parse_stdfmt_format_spec(__sv);

        // Validate any extra arguments consumed (i.e runtime width and precision values).
        if (_M_spec.__width == __stdfmt_wp_type::__runtime_arg_implicit)
            _M_spec.__width_val = __pc.next_arg_id();
        else if (_M_spec.__width == __stdfmt_wp_type::__runtime_arg_explicit)
            __pc.check_arg_id(_M_spec.__width_val);

        if (_M_spec.__precision == __stdfmt_wp_type::__runtime_arg_implicit)
            _M_spec.__precision_val = __pc.next_arg_id();
        else if (_M_spec.__precision == __stdfmt_wp_type::__runtime_arg_explicit)
            __pc.check_arg_id(_M_spec.__precision_val);

        return __pc.end();
    }

    constexpr bool __is_integral_presentation() const noexcept
    {
        switch (_M_spec.__type)
        {
            case __stdfmt_presentation_type::__b:
            case __stdfmt_presentation_type::__B:
            case __stdfmt_presentation_type::__d:
            case __stdfmt_presentation_type::__o:
            case __stdfmt_presentation_type::__x:
            case __stdfmt_presentation_type::__X:
                return true;

            default:
                return false;
        }
    }

    template <class _T>
    constexpr bool __is_arithmetic_type() const noexcept
    {
        return std::is_pointer_v<_T> || (std::is_arithmetic_v<_T> && !std::is_same_v<_T, bool> && !std::is_same_v<_T, _CharT>);
    }

    template <class _T>
    constexpr bool __treat_as_arithmetic() const noexcept
    {
        if (__is_integral_presentation())
            return true;

        if (_M_spec.__type == __stdfmt_presentation_type::__c)
            return false;

        return __is_arithmetic_type<_T>();
    }

    // Resolves the default presentation type for the given argument type.
    template <class _T>
    constexpr void __resolve_presentation() noexcept
    {
        // If we have a non-default presentation, leave it alone.
        if (_M_spec.__type != __stdfmt_presentation_type::__default)
            return;

        if constexpr (std::is_same_v<_T, const _CharT*> || std::is_same_v<_T, std::basic_string_view<_CharT>>)
            _M_spec.__type = __stdfmt_presentation_type::__s;
        else if constexpr (std::is_same_v<_T, _CharT>)
            _M_spec.__type = __stdfmt_presentation_type::__c;
        else if constexpr (std::is_same_v<_T, bool>)
            _M_spec.__type = __stdfmt_presentation_type::__s;
        else if constexpr (std::is_integral_v<_T>)
            _M_spec.__type = __stdfmt_presentation_type::__d;
        else if constexpr (std::is_floating_point_v<_T>)
        {
            // Differs based on whether precision is specified.
            if (_M_spec.__precision == __stdfmt_wp_type::__none)
                _M_spec.__type = __stdfmt_presentation_type::__floating_point;
            else
                _M_spec.__type = __stdfmt_presentation_type::__g;
        }
        else if constexpr (std::is_same_v<_T, const void*>)
            _M_spec.__type = __stdfmt_presentation_type::__p;
        else
        {
            // Programming error in this module!
            static_assert(!is_same_v<_T, _T>, "internal error: bad type passed to standard lib formatter");
        }
    }

    // Resolves the default alignment, which depends on the argument type.
    template <class _T>
    constexpr void __resolve_alignment() noexcept
    {
        if (_M_spec.__align != __stdfmt_align_type::__default)
            return;

        _M_spec.__implicit_alignment = true;

        if (__treat_as_arithmetic<_T>())
            _M_spec.__align = __stdfmt_align_type::__end;
        else
            _M_spec.__align = __stdfmt_align_type::__begin;
    }

    // Resolves any runtime-specified width and precision values.
    template <class _Out>
    constexpr void __resolve_wp(_FC<_Out>& __fmt)
    {
        if (_M_spec.__width == __stdfmt_wp_type::__runtime_arg_explicit || _M_spec.__width == __stdfmt_wp_type::__runtime_arg_implicit)
        {
            auto __width = visit_format_arg([]<class _Arg>(const _Arg& __arg) constexpr -> std::size_t
            {
                if constexpr (!std::is_integral_v<_Arg> || std::is_same_v<_Arg, bool>)
                    throw format_error("invalid width argument: not an integral type");
                else
                {
                    if (__arg <= 0)
                        throw format_error("invalid width argument: must be a positive value");

                    auto __uarg = static_cast<std::make_unsigned_t<_Arg>>(__arg);
                    if (__uarg > std::numeric_limits<std::size_t>::max())
                        throw format_error("invalid width argument: overflow");

                    return static_cast<std::size_t>(__uarg);
                }
            }, __fmt.arg(_M_spec.__width_val));

            _M_spec.__width = __stdfmt_wp_type::__fixed;
            _M_spec.__width_val = __width;
        }

        if (_M_spec.__precision == __stdfmt_wp_type::__runtime_arg_explicit || _M_spec.__precision == __stdfmt_wp_type::__runtime_arg_implicit)
        {
            auto __precision = visit_format_arg([]<class _Arg>(const _Arg& __arg) constexpr -> std::size_t
            {
                if constexpr (!std::is_integral_v<_Arg> || std::is_same_v<_Arg, bool>)
                    throw format_error("invalid precision argument: not an integral type");
                else
                {
                    if (__arg < 0)
                        throw format_error("invalid precision argument: must be a non-negative value");

                    auto __uarg = static_cast<std::make_unsigned_t<_Arg>>(__arg);
                    if (__uarg > std::numeric_limits<std::size_t>::max())
                        throw format_error("invalid precision argument: overflow");

                    return static_cast<std::size_t>(__uarg);
                }
            }, __fmt.arg(_M_spec.__precision_val));

            _M_spec.__precision = __stdfmt_wp_type::__fixed;
            _M_spec.__precision_val = __precision;
        }
    }

    // Checks that the spec is valid for the argument type (except the presentation type).
    template <class _T>
    constexpr void __check_spec() const
    {
        // Sign is only allowed for arithmetic types or integral presentation.
        if (!__treat_as_arithmetic<_T>() && _M_spec.__sign != __stdfmt_sign_type::__implicit_default)
            throw format_error("non-arithmetic fields cannot have signs");

        // Zero padding is only allowed for arithmetic types or integral presentation.
        if (!__treat_as_arithmetic<_T>() && _M_spec.__zero_pad)
            throw format_error("non-arithmetic fields cannot be zero-padded");

        // Alternate form is only allowed for arithmetic types or integral presentation.
        if (!__treat_as_arithmetic<_T>() && _M_spec.__alt_form)
            throw format_error("non-arithmetic fields cannot be printed in alternate form");

        // Precision is only allowed for floating-point types and strings.
        if (_M_spec.__precision != __stdfmt_wp_type::__none && !std::is_floating_point_v<_T> && _M_spec.__type != __stdfmt_presentation_type::__s)
            throw format_error("precision is valid only for floating-point types or string presentation");

        // Locale-specific forms are only allowed for arithmetic types (including bool string presentation).
        if (_M_spec.__locale_specific && (!std::is_arithmetic_v<_T> || _M_spec.__type == __stdfmt_presentation_type::__c))
            throw format_error("locale-specific output is valid only for arithmetic types and bool string presentation");
    }

    template <class _T, class _Out>
    constexpr void __prepare(_FC<_Out>& __fmt)
    {
        __resolve_presentation<_T>();
        __resolve_alignment<_T>();
        __resolve_wp(__fmt);
        __check_spec<_T>();
    }

    template <class _Out>
    constexpr _Out __do_output(_Out __out, std::basic_string_view<_CharT> __formatted)
    {
        using __len_t = std::iter_difference_t<_Out>;

        // Calculate the amount of fill needed.
        __len_t __fill_size = 0;
        if (_M_spec.__width != __stdfmt_wp_type::__none)
        {
            //! @todo the spec requires this to be similar to wcswidth, not an element count.
            auto __est_width = __formatted.size();

            if (__est_width < _M_spec.__width_val)
                __fill_size = static_cast<__len_t>(_M_spec.__width_val - __est_width);
        }

        // Split the fill into pre-fill and post-fill, based on the requested alignment type.
        __len_t __pre_fill = 0;
        __len_t __post_fill = 0;
        switch (_M_spec.__align)
        {
            case __stdfmt_align_type::__begin:
                __post_fill = __fill_size;
                break;

            case __stdfmt_align_type::__middle:
                // Pre-fill is N/2, rounded down. Post-fill is N/2, rounded up.
                __pre_fill = (__fill_size / 2);
                __post_fill = __fill_size - __pre_fill;
                break;

            case __stdfmt_align_type::__end:
                __pre_fill = __fill_size;
                break;

            case __stdfmt_align_type::__default:
                // This case won't happen; the default alignment type should have been resolved already.
                break;
        }

        // Output any pre-fill.
        __out = std::ranges::fill_n(__out, __pre_fill, _M_spec.__fill_char);

        // Output the formatted data.
        __out = std::ranges::copy(__formatted, __out).out;

        // Output any post-fil.
        __out = std::ranges::fill_n(__out, __post_fill, _M_spec.__fill_char);

        return __out;
    }

    template <class _Out>
    constexpr _Out __format_bool(_Out __out, bool __b)
    {
        //! @todo: support for locale-dependent strings.
        auto __str = __b ? "true" : "false";

        // Widen the string.
        _CharT __arr[6];
        auto* __ptr = __arr; 
        while (*__str)
            *__ptr++ = static_cast<_CharT>(*__str++);

        // We pass to the string formatting method so precisions still get applied.
        return __format_string_view(std::move(__out), std::basic_string_view<_CharT>{__arr, static_cast<std::size_t>(__ptr - __arr)});
    }

    template <class _Out, class _T>
    constexpr _Out __format_char(_Out __out, _T __c)
    {
        _CharT __arr[2] = { static_cast<_CharT>(__c), 0 };
        return __do_output(std::move(__out), std::basic_string_view<_CharT>(__arr));   
    }

    template <class _Out>
    constexpr _Out __format_cstring(_Out __out, const _CharT* __s)
    {
        // If we have a precision, we use the smaller of that and the string length for the length of the string. Else,
        // we use the whole string. We need to delay the strlen as the input might not be terminated.
        std::size_t __length;
        if (_M_spec.__precision == __stdfmt_wp_type::__none)
        {
            __length = std::char_traits<_CharT>::length(__s);
        }
        else
        {
            auto __end = std::char_traits<_CharT>::find(__s, _M_spec.__precision_val, 0);
            __length = __end ? static_cast<std::size_t>(__end - __s) : _M_spec.__precision_val;
        }

        return __do_output(std::move(__out), std::basic_string_view<_CharT>(__s, __length));
    }

    template <class _Out>
    constexpr _Out __format_string_view(_Out __out, std::basic_string_view<_CharT> __sv)
    {
        // If we have a precision and it is smaller than the input, we only print out that part of the input.
        if (_M_spec.__precision != __stdfmt_wp_type::__none)
            __sv = __sv.substr(0, _M_spec.__precision_val);

        return __do_output(std::move(__out), __sv);
    }

    template <class _Out, class _T>
    constexpr _Out __format_integer(_Out __out, _T __n)
    {
        //! @todo: support for locale-dependent formatting.

        // The maximum length is 2 chars for the alt form prefix plus one for the sign plus one char per bit.
        constexpr std::size_t _MaxLen = 2 + 1 + std::numeric_limits<_T>::digits;

        // We format the number from least- to most-significant digit.
        _CharT __arr[_MaxLen];
        _CharT* __end = &__arr[_MaxLen - 1];
        _CharT* __ptr = __end;

        // Determine the base and the alt-form prefix, if needed.
        const char* __prefix = nullptr;
        unsigned int __base;
        bool __upper = false;
        switch (_M_spec.__type)
        {
            case __stdfmt_presentation_type::__b:
                __base = 2;
                if (_M_spec.__alt_form)
                   __prefix = "0b";
                break;

            case __stdfmt_presentation_type::__B:
                __base = 2;
                if (_M_spec.__alt_form)
                    __prefix = "0B";
                break;

            case __stdfmt_presentation_type::__d:
                __base = 10;
                break;

            case __stdfmt_presentation_type::__o:
                __base = 8;
                if (_M_spec.__alt_form && __n != 0)
                    __prefix = "0";
                break;

            case __stdfmt_presentation_type::__x:
                __base = 16;
                if (_M_spec.__alt_form)
                    __prefix = "0x";
                break;

            case __stdfmt_presentation_type::__X:
                __base = 16;
                __upper = true;
                if (_M_spec.__alt_form)
                    __prefix = "0X";
                break;

            default:
                // Shouldn't happen.
                __base = 10;
                break;
        }

        // Calculate the sign.
        using _UT = std::make_unsigned_t<_T>;
        _CharT __sign = 0;
        _UT __un;
        if (__n < 0)
        {
            __sign = static_cast<_CharT>('-');
            __un = static_cast<_UT>(-__n);
        }
        else
        {
            __un = static_cast<_UT>(__n);
            if (_M_spec.__sign == __stdfmt_sign_type::__always)
                __sign = static_cast<_CharT>('+');
            else if (_M_spec.__sign == __stdfmt_sign_type::__leading_space) 
                __sign = static_cast<_CharT>(' ');
        }

        // Write the output.
        if (__un != 0)
        {
            constexpr auto __convL = "0123456789abcdef";
            constexpr auto __convU = "0123456789ABCDEF";

            const char* __conv = __upper ? __convU : __convL;

            while (__un > 0)
            {
                *__ptr-- = static_cast<_CharT>(__conv[__un % __base]);
                __un /= __base;
            }
        }
        else
        {
            // Special case for zero inputs.
            *__ptr-- = static_cast<_CharT>('0');
        }

        // Configure zero-fill if requested and some other feature doesn't take precedence.
        if (_M_spec.__zero_pad && _M_spec.__implicit_alignment)
        {
            std::size_t __prefix_len = 0;
        
            // Add the sign, if present.
            if (__sign)
            {
                *__out = __sign;
                ++__out;
                ++__prefix_len;
                __sign = 0;
            }


            // We need to handle this case specially - the zeroes go between the prefix and the formatted number. We'll
            // use the normal fill logic but only after having written out the prefix.
            if (__prefix)
            {
                *__out = static_cast<_CharT>(__prefix[0]);
                ++__out;
                ++__prefix_len;

                if (__prefix[1] != 0)
                {
                    *__out = static_cast<_CharT>(__prefix[1]);
                    ++__out;
                    ++__prefix_len;
                }

                // We've already written the prefix so don't add it to the output buffer.
                __prefix = nullptr;
            }

            // Reduce the field with by the length of the prefix we wrote (making sure to keep it positive).
            _M_spec.__width_val = _M_spec.__width_val > __prefix_len ? _M_spec.__width_val - __prefix_len : 1;

            // Fill as normal, but using zeroes instead of spaces.
            _M_spec.__fill_char = static_cast<_CharT>('0');
        }

        // Add the prefix, if present.
        if (__prefix)
        {
            if (__prefix[1] != 0)
                *__ptr-- = static_cast<_CharT>(__prefix[1]);
            *__ptr-- = static_cast<_CharT>(__prefix[0]);
        }

        // Add the sign, if present.
        if (__sign)
            *__ptr-- = __sign;

        return __do_output(std::move(__out), std::basic_string_view<_CharT>{__ptr + 1, static_cast<std::size_t>(__end - __ptr)});
    }

    template <class _Out, class _T>
    constexpr _Out __format_float(_Out __out, _T __f)
    {
        //! @todo: implement
        return std::move(__out);
    }

    template <class _Out>
    constexpr _Out __format_pointer(_Out __out, const void* __p)
    {
        // Force alt form on and print as a base-16 value.
        _M_spec.__alt_form = true;
        _M_spec.__type = __stdfmt_presentation_type::__x;

        return __format_integer(std::move(__out), reinterpret_cast<std::uintptr_t>(__p));
    }
};


template <class _CharT>
struct __bool_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(bool __b, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<bool>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__s:
                return this->__format_bool(__fc.out(), __b);

            case __stdfmt_presentation_type::__b:
            case __stdfmt_presentation_type::__B:
            case __stdfmt_presentation_type::__d:
            case __stdfmt_presentation_type::__o:
            case __stdfmt_presentation_type::__x:
            case __stdfmt_presentation_type::__X:
                return this->__format_integer(__fc.out(), static_cast<unsigned char>(__b));

            default:
                throw format_error("invalid presentation type for bool argument");
        }
    }
};


template <class _CharT, class _T>
struct __char_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(_T __t, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<_T>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__c:
                return this->__format_char(__fc.out(), __t);

            case __stdfmt_presentation_type::__b:
            case __stdfmt_presentation_type::__B:
            case __stdfmt_presentation_type::__d:
            case __stdfmt_presentation_type::__o:
            case __stdfmt_presentation_type::__x:
            case __stdfmt_presentation_type::__X:
                return this->__format_integer(__fc.out(), __t);

            default:
                throw format_error("invalid presentation type for a character-type argument");
        }
    }
};

template <class _CharT>
struct __cstring_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(const _CharT* __s, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<const _CharT*>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__s:
                return this->__format_cstring(__fc.out(), __s);

            default:
                throw format_error("invalid presentation type for a string-type argument");
        }
    }
};

template <class _CharT, std::size_t _N>
struct __char_array_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(const _CharT (&__arr)[_N], basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<std::basic_string_view<_CharT>>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__s:
                return this->__format_string_view(__fc.out(), std::basic_string_view<_CharT>{__arr, _N});

            default:
                throw format_error("invalid presentation type for a string-type argument");
        }
    }
};

template <class _CharT>
struct __string_view_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(std::basic_string_view<_CharT> __sv, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<std::basic_string_view<_CharT>>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__s:
                return this->__format_string_view(__fc.out(), __sv);

            default:
                throw format_error("invalid presentation type for a string-type argument");
        }
    }
};

template <class _CharT, class _T>
struct __integer_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(_T __t, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<_T>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__c:
                if (static_cast<_T>(static_cast<_CharT>(__t)) != __t)
                    throw format_error("numeric value outside range of valid character values");
                return this->__format_char(__fc.out(), static_cast<_CharT>(__t));

            case __stdfmt_presentation_type::__b:
            case __stdfmt_presentation_type::__B:
            case __stdfmt_presentation_type::__d:
            case __stdfmt_presentation_type::__o:
            case __stdfmt_presentation_type::__x:
            case __stdfmt_presentation_type::__X:
                return this->__format_integer(__fc.out(), __t);

            default:
                throw format_error("invalid presentation type for an integer-type argument");
        }
    }
};

template <class _CharT, class _T>
struct __float_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(_T __t, basic_format_context<_Out, _CharT>& __fc)
    {
        //! @todo: implement.
        (void)__t;
        return std::move(__fc.out());
    }
};

template <class _CharT, class _T>
struct __pointer_formatter : __stdfmt_formatter_base<_CharT>
{
    template <class _Out>
    constexpr _Out format(_T __t, basic_format_context<_Out, _CharT>& __fc)
    {
        __stdfmt_formatter_base<_CharT>::template __prepare<_T>(__fc);

        switch (__stdfmt_formatter_base<_CharT>::_M_spec.__type)
        {
            case __stdfmt_presentation_type::__p:
                return this->__format_pointer(__fc.out(), __t);

            default:
                throw format_error("invalid presentation type for a pointer argument");
        }
    }
};


} // namespace __detail


// By default, all non-specialised formatters are disabled.
template <class _T, class _CharT>
struct formatter : public __detail::__disabled_formatter {};

template <> struct formatter<char, char> : __detail::__char_formatter<char, char> {};

template <> struct formatter<char, wchar_t> : __detail::__char_formatter<wchar_t, char> {};

template <> struct formatter<wchar_t, wchar_t> : __detail::__char_formatter<wchar_t, wchar_t> {};

template <> struct formatter<char*, char> : __detail::__cstring_formatter<char> {};
template <> struct formatter<const char*, char> : __detail::__cstring_formatter<char> {};
template <std::size_t _N> struct formatter<const char[_N], char> : __detail::__char_array_formatter<char, _N> {};
template <class _Traits, class _Allocator> struct formatter<std::basic_string<char, _Traits, _Allocator>, char> : __detail::__string_view_formatter<char> {};
template <class _Traits> struct formatter<std::basic_string_view<char, _Traits>, char> : __detail::__string_view_formatter<char> {};

template <> struct formatter<wchar_t*, wchar_t> : __detail::__cstring_formatter<wchar_t> {};
template <> struct formatter<const wchar_t*, wchar_t> : __detail::__cstring_formatter<wchar_t> {};
template <std::size_t _N> struct formatter<const wchar_t[_N], wchar_t> : __detail::__char_array_formatter<wchar_t, _N> {};
template <class _Traits, class _Allocator> struct formatter<std::basic_string<wchar_t, _Traits, _Allocator>, wchar_t> : __detail::__string_view_formatter<wchar_t> {};
template <class _Traits> struct formatter<std::basic_string_view<wchar_t, _Traits>, wchar_t> : __detail::__string_view_formatter<wchar_t> {};

template <> struct formatter<bool, char> : __detail::__bool_formatter<char> {};
template <> struct formatter<signed char, char> : __detail::__integer_formatter<char, signed char> {};
template <> struct formatter<unsigned char, char> : __detail::__integer_formatter<char, unsigned char> {};
template <> struct formatter<signed short, char> : __detail::__integer_formatter<char, signed short> {};
template <> struct formatter<unsigned short, char> : __detail::__integer_formatter<char, unsigned short> {};
template <> struct formatter<signed int, char> : __detail::__integer_formatter<char, signed int> {};
template <> struct formatter<unsigned int, char> : __detail::__integer_formatter<char, unsigned int> {};
template <> struct formatter<signed long, char> : __detail::__integer_formatter<char, signed long> {};
template <> struct formatter<unsigned long, char> : __detail::__integer_formatter<char, unsigned long> {};
template <> struct formatter<signed long long, char> : __detail::__integer_formatter<char, signed long long> {};
template <> struct formatter<unsigned long long, char> : __detail::__integer_formatter<char, unsigned long long> {};
template <> struct formatter<float, char> : __detail::__float_formatter<char, float> {};
template <> struct formatter<double, char> : __detail::__float_formatter<char, double> {};
template <> struct formatter<long double, char> : __detail::__float_formatter<char, long double> {};

template <> struct formatter<bool, wchar_t> : __detail::__bool_formatter<wchar_t> {};
template <> struct formatter<signed char, wchar_t> : __detail::__integer_formatter<wchar_t, signed char> {};
template <> struct formatter<unsigned char, wchar_t> : __detail::__integer_formatter<wchar_t, unsigned char> {};
template <> struct formatter<signed short, wchar_t> : __detail::__integer_formatter<wchar_t, signed short> {};
template <> struct formatter<unsigned short, wchar_t> : __detail::__integer_formatter<wchar_t, unsigned short> {};
template <> struct formatter<signed int, wchar_t> : __detail::__integer_formatter<wchar_t, signed int> {};
template <> struct formatter<unsigned int, wchar_t> : __detail::__integer_formatter<wchar_t, unsigned int> {};
template <> struct formatter<signed long, wchar_t> : __detail::__integer_formatter<wchar_t, signed long> {};
template <> struct formatter<unsigned long, wchar_t> : __detail::__integer_formatter<wchar_t, unsigned long> {};
template <> struct formatter<signed long long, wchar_t> : __detail::__integer_formatter<wchar_t, signed long long> {};
template <> struct formatter<unsigned long long, wchar_t> : __detail::__integer_formatter<wchar_t, unsigned long long> {};
template <> struct formatter<float, wchar_t> : __detail::__float_formatter<wchar_t, float> {};
template <> struct formatter<double, wchar_t> : __detail::__float_formatter<wchar_t, double> {};
template <> struct formatter<long double, wchar_t> : __detail::__float_formatter<wchar_t, long double> {};

template <> struct formatter<std::nullptr_t, char> : __detail::__pointer_formatter<char, std::nullptr_t> {};
template <> struct formatter<void*, char> : __detail::__pointer_formatter<char, const void*> {};
template <> struct formatter<const void*, char> : __detail::__pointer_formatter<char, const void*> {};

template <> struct formatter<std::nullptr_t, wchar_t> : __detail::__pointer_formatter<wchar_t, std::nullptr_t> {};
template <> struct formatter<void*, wchar_t> : __detail::__pointer_formatter<wchar_t, const void*> {};
template <> struct formatter<const void*, wchar_t> : __detail::__pointer_formatter<wchar_t, const void*> {};


namespace __detail
{


template <class _Out, class _CharT, bool _OnlyParse = false>
constexpr _Out __parse_and_format_to(_Out __out, std::basic_string_view<_CharT> __fmt, basic_format_args<basic_format_context<_Out, _CharT>> __args)
{
    // Parse context.
    basic_format_parse_context<_CharT> __parse {__fmt, __args.__size()};

    // Format context.
    //
    // Note that the format context takes ownership of the output iterator.
    using _FC = basic_format_context<_Out, _CharT>;
    auto __fc = _FC::__make(__args, std::move(__out));

    // Iterate over the literal output and replacement fields.
    while (!__fmt.empty())
    {
        // Find the next replacement and all the text before it.
        auto __field = __detail::__find_next_replacement_field(__fmt);
        auto __before = __fmt.substr(0, static_cast<std::size_t>(__field.begin() - __fmt.begin()));

        // Output the literal text. This also takes back the output iterator from the format context.
        if constexpr (!_OnlyParse)
            __out = (std::ranges::copy(__before, __fc.out())).out;

        // If the replacement field is empty, we are done.
        if (__field.empty())
            break;

        // This is a replacement field. Perform a type-independent parse on it so we know which argument to use to
        // create the type-dependent parser.
        auto __info = __detail::__parse_replacement_field(__field);

        // Check for literal replacements.
        if (__info.__replacement_char)
        {
            if constexpr (!_OnlyParse)
            {
                // Just write out the literal.
                *__out = __info.__replacement_char;
                ++__out;

                // Update the formatting context so that the iterator is in the same place in both cases.
                __fc.advance_to(std::move(__out));
            }
        }
        else
        {
            // This field depends on an argument. Get the index of the argument that we're going to format.
            std::size_t __arg_idx;
            if (__info.__arg_id_type == __detail::__replacement_arg_id_type::__explicit)
            {
                // Check that the explicit argument ID is valid.
                __parse.check_arg_id(__info.__arg_id);
                __arg_idx = __info.__arg_id;
            }
            else
            {
                // Get the index of the next implicit argument.
                __arg_idx = __parse.next_arg_id();
            }

            // Update the contexts.
            __parse.__set(__info.__format_spec);
            __fc.advance_to(std::move(__out));

            // Visit the argument so that we can use the appropriate formatter to parse the input and format the output.
            visit_format_arg([&]<class _T>(const _T& __arg)
            {
                if constexpr (std::is_same_v<_T, bool>)
                {
                    std::formatter<bool, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, _CharT>)
                {
                    std::formatter<_CharT, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, int>)
                {
                    std::formatter<int, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, unsigned int>)
                {
                    std::formatter<unsigned int, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, long long>)
                {
                    std::formatter<long long, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, unsigned long long>)
                {
                    std::formatter<unsigned long long, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, float>)
                {
                    std::formatter<float, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, double>)
                {
                    std::formatter<double, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, long double>)
                {
                    std::formatter<long double, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, const _CharT*>)
                {
                    std::formatter<const _CharT*, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, std::basic_string_view<_CharT>>)
                {
                    std::formatter<std::basic_string_view<_CharT>, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, const void*>)
                {
                    std::formatter<const void*, _CharT> __f;
                    __f.parse(__parse);
                    if constexpr (!_OnlyParse)
                        __fc.advance_to(__f.format(__arg, __fc));
                }
                else if constexpr (std::is_same_v<_T, typename basic_format_arg<_FC>::handle>)
                {
                    if constexpr (_OnlyParse)
                        __arg.__parse(__parse);
                    else
                        __arg.format(__parse, __fc);
                }
                else
                {
                    // Programming error inside this formatting library...
                    static_assert(!std::is_same_v<_T, _T>, "logic error in formatting library");
                }
            }, __fc.arg(__arg_idx));
        }

        // This field has been processed.
        __fmt = __fmt.substr(static_cast<std::size_t>(__field.end() - __fmt.begin()));
    }

    return __fc.out();
}


template <class _CharT>
class __stdfmt_dummy_iter
{
public:

    using difference_type = std::ptrdiff_t;

    constexpr _CharT& operator*() noexcept { return _M_ignored; }

    constexpr __stdfmt_dummy_iter& operator++() noexcept { return *this; }

    constexpr __stdfmt_dummy_iter operator++(int) noexcept
    {
        auto __copy = *this;
        operator++();
        return __copy;
    }

private:

    _CharT _M_ignored = {};
};


template <class _CharT>
consteval void __consteval_parse_impl(std::basic_string_view<_CharT> __fmt, basic_format_args<basic_format_context<__stdfmt_dummy_iter<_CharT>, _CharT>> __args)
{
    // Evaluated for side-effects only, i.e does it throw?
    __parse_and_format_to<__stdfmt_dummy_iter<_CharT>, _CharT, true>(__stdfmt_dummy_iter<_CharT>{}, __fmt, __args);
}


template <class> struct __fmt_arg_type_extractor {};
template <__fmt_arg_type _T> struct __fmt_arg_type_extractor<__fmt_arg_type_tag<_T>> { static constexpr auto __value = _T; };


template <class _CharT, class _Arg>
consteval decltype(auto) __generate_dummy_format_arg()
{
    using _ArgT = std::remove_cvref_t<_Arg>;

    // If we were to attempt to store an arg of type _Arg, what 'kind' would it be?
    using _Context = basic_format_context<__stdfmt_dummy_iter<_CharT>, _CharT>;
    using _TypeTag = decltype(__store_format_arg<_Context>(declval<_Arg>(), declval<__fmt_arg_type&>(), declval<__basic_format_arg_storage<_CharT>&>()));
    constexpr auto _Type = __fmt_arg_type_extractor<_TypeTag>::__value;

    // For anything except handles, we can trivially generate a default-constructed value and be sure of no side-effects
    // or other weird non-consteval behaviour.
    if constexpr (_Type == __fmt_arg_type::__handle)
    {
        // Oh dear; this is a non-standard type with a user-defined formatter.
        //
        // We need to return something with the correct type, so that we'll use the right parser when we generate the
        // type-erased handle. If we can trivially default-construct the type, go ahead and do that.
        if constexpr (std::is_trivially_default_constructible_v<_Arg>)
        {
            return _ArgT{};
        }
        else
        {
            // Be very naughty and return a reference obtained by dereferencing a null pointer...
            _ArgT* __naughty = nullptr;
            return static_cast<_Arg>(*__naughty);
        }
    }
    else
    {
        // Just go ahead and construct the arg as normal.
        return _ArgT{};
    }
}


template <class _CharT, class... _Args>
consteval void __consteval_parse(std::basic_string_view<_CharT> __fmt)
{
    // Generate placeholder values for all args.
    using _Context = basic_format_context<__stdfmt_dummy_iter<_CharT>, _CharT>;
    __consteval_parse_impl(__fmt, basic_format_args<_Context>(make_format_args<_Context>(std::forward<_Args>(__generate_dummy_format_arg<_CharT, _Args>())...)));
}


template <class _CharT>
class __basic_fmt_measuring_iter
{
public:

    using value_type        = _CharT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = _CharT*;
    using reference         = _CharT&;
    using iterator_category = std::output_iterator_tag;

    constexpr __basic_fmt_measuring_iter() = default;

    constexpr reference operator*() const
    {
        return _M_dummy;
    }

    constexpr __basic_fmt_measuring_iter& operator++()
    {
        ++_M_count;
        return *this;
    }

    constexpr __basic_fmt_measuring_iter& operator++(int)
    {
        ++_M_count;
        return *this;
    }

    constexpr std::size_t __count() const noexcept
    {
        return _M_count;
    }

private:

    std::size_t     _M_count = 0;
    mutable _CharT  _M_dummy;
};


template <class _CharT>
constexpr std::size_t __vformatted_size(std::basic_string_view<_CharT> __fmt, basic_format_args<basic_format_context<__basic_fmt_measuring_iter<_CharT>, _CharT>> __args)
{
    __basic_fmt_measuring_iter<_CharT> __out {};

    __out = __parse_and_format_to(std::move(__out), __fmt, __args);

    return __out.__count();
}


template <class _CharT, class... _Args>
constexpr std::size_t __formatted_size(std::basic_string_view<_CharT> __fmt, _Args&&... __args)
{
    using _Context = basic_format_context<__basic_fmt_measuring_iter<_CharT>, _CharT>;
    return __vformatted_size(__fmt, basic_format_args<_Context>{make_format_args<_Context>(std::forward<_Args>(__args)...)});
}


} // namespace __detail


template <class _CharT, class... _Args>
template <class _T>
    requires std::convertible_to<const _T&, std::basic_string_view<_CharT>>
consteval basic_format_string<_CharT, _Args...>::basic_format_string(const _T& __s) :
    _M_str{__s}
{
    // Evaluate this for side-effects (i.e throwing if parsing fails).
    __detail::__consteval_parse<_CharT, _Args...>(_M_str);
}


constexpr std::string vformat(std::string_view __fmt, format_args __args)
{
    // Target.
    std::string __formatted;
    auto __out = std::back_inserter(__formatted);

    __detail::__parse_and_format_to(std::move(__out), __fmt, __args);

    return __formatted;
}


constexpr std::wstring vformat(std::wstring_view __fmt, wformat_args __args)
{
    // Target.
    std::wstring __formatted;
    auto __out = std::back_inserter(__formatted);

    __detail::__parse_and_format_to(std::move(__out), __fmt, __args);

    return __formatted;
}


template <class _Out>
constexpr _Out vformat_to(_Out __out, std::string_view __fmt, format_args __args)
{
    return __detail::__parse_and_format_to(std::move(__out), __fmt, __args);
}


template <class _Out>
constexpr _Out vformat_to(_Out __out, std::wstring_view __fmt, wformat_args __args)
{
    return __detail::__parse_and_format_to(std::move(__out), __fmt, __args);
}


template <class... _Args>
constexpr std::string format(format_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat(__fmt.get(), make_format_args(std::forward<_Args>(__args)...));
}


template <class... _Args>
constexpr std::wstring format(wformat_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat(__fmt.get(), make_wformat_args(std::forward<_Args>(__args)...));
}


template <class... _Args>
constexpr std::string format(const locale& __loc, format_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat(__loc, __fmt.get(), make_format_args(std::forward<_Args>(__args)...));
}


template <class... _Args>
constexpr std::wstring format(const locale& __loc, wformat_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat(__loc, __fmt.get(), make_wformat_args(std::forward<_Args>(__args)...));
}


template <class _Out, class... _Args>
constexpr _Out format_to(_Out __out, format_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat_to(std::move(__out), __fmt.get(), make_format_args(std::forward<_Args>(__args)...));
}


template <class _Out, class... _Args>
constexpr _Out format_to(_Out __out, wformat_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat_to(std::move(__out), __fmt.get(), make_wformat_args(std::forward<_Args>(__args)...));
}


template <class _Out, class... _Args>
constexpr _Out format_to(_Out __out, const locale& __loc, format_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat_to(std::move(__out), __loc, __fmt.get(), make_format_args(std::forward<_Args>(__args)...));
}


template <class _Out, class... _Args>
constexpr _Out format_to(_Out __out, const locale& __loc, wformat_string<_Args...> __fmt, _Args&&... __args)
{
    return vformat_to(std::move(__out), __loc, __fmt.get(), make_wformat_args(std::forward<_Args>(__args)...));
}


template <class... _Args> constexpr std::size_t formatted_size(format_string<_Args...> __fmt, _Args&&... __args)
{
    return __detail::__formatted_size(__fmt.get(), std::forward<_Args>(__args)...);
}

template <class... _Args> constexpr std::size_t formatted_size(wformat_string<_Args...> __fmt, _Args&&... __args)
{
    return __detail::__formatted_size(__fmt.get(), std::forward<_Args>(__args)...);
}


} // namespace __XVI_STD_FORMAT_NS


#endif /* ifndef __SYSTEM_CXX_FORMAT_FORMAT_H */

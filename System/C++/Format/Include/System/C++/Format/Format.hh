#ifndef __SYSTEM_CXX_FORMAT_FORMAT_H
#define __SYSTEM_CXX_FORMAT_FORMAT_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Locale/CType.hh>
#include <System/C++/Locale/Locale.hh>
#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/CharConv.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/Span.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/String.hh>
#include <System/C++/Utility/StringView.hh>
#include <System/C++/Utility/Variant.hh>

#include <System/C++/Format/Private/Config.hh>


namespace __XVI_STD_FORMAT_NS
{


// Forward declarations.
template <class _T, class _CharT = char> struct formatter;
template <class _CharT> class basic_format_parse_context;
template <class _Context> class basic_format_arg;
template <class _Context> class basic_format_args;
template <class _Out, class _Context> class basic_format_context;


namespace __detail
{


template <class _CharT>
class __basic_contiguous_append_iterator
{
public:

    using value_type        = _CharT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = _CharT*;
    using reference         = _CharT&;
    using iterator_category = std::output_iterator_tag;

    constexpr __basic_contiguous_append_iterator() = default;
    constexpr __basic_contiguous_append_iterator(const __basic_contiguous_append_iterator& __x) :
        _M_span(__x._M_span),
        _M_container(__x._M_container),
        _M_extend(__x._M_extend)
    {
    }
    constexpr __basic_contiguous_append_iterator& operator=(const __basic_contiguous_append_iterator&) = default;
    constexpr ~__basic_contiguous_append_iterator() = default;

    template <class _Container>
        requires (!is_same_v<remove_cv_t<_Container>, __basic_contiguous_append_iterator>
                  && contiguous_iterator<typename _Container::iterator>)
    constexpr __basic_contiguous_append_iterator(_Container& __c) :
        _M_span{__c.data() + __c.size(), 0},
        _M_container{std::addressof(__c)},
        _M_extend{[](void* __cont, std::size_t __more) -> std::span<_CharT>
        {
            auto __ptr = reinterpret_cast<_Container*>(__cont);
            __ptr->resize(__ptr->size() + __more);
            return {__ptr->data() +__ptr->size() - __more, __more};
        }}
    {
    }

    constexpr reference operator*() const
    {
        if (_M_span.size() == 0)
            __extend();

        return _M_span[0];
    }

    constexpr __basic_contiguous_append_iterator& operator++()
    {
        _M_span = _M_span.subspan(1);
        return *this;
    }

    constexpr __basic_contiguous_append_iterator operator++(int)
    {
        auto __tmp = *this;
        ++*this;
        return __tmp;
    }

private:

    using __extend_fn = std::span<_CharT> (*)(void*, std::size_t);

    mutable std::span<_CharT>   _M_span         = {};
    void*                       _M_container    = nullptr;
    __extend_fn                 _M_extend       = nullptr;

    constexpr void __extend(std::size_t __extra_size = 1) const
    {
        _M_span = _M_extend(_M_container, __extra_size);
    }
};

using __contiguous_append_iterator = __basic_contiguous_append_iterator<char>;
using __contiguous_wappend_iterator = __basic_contiguous_append_iterator<wchar_t>;


template <class _Out, class _CharT>
class __basic_truncating_iterator
{
public:

    using value_type        = typename _Out::value_type;
    using difference_type   = typename _Out::difference_type;
    using pointer           = typename _Out::pointer;
    using reference         = typename _Out::reference;
    using iterator_category = std::output_iterator_tag;

    constexpr __basic_truncating_iterator() = default;

    constexpr __basic_truncating_iterator(_Out __iter, std::size_t __remaining) :
        _M_iter(__iter),
        _M_remaining(__remaining)
    {
    }

    constexpr reference operator*() const
    {
        if (_M_remaining == 0)
            return _M_dummy;

        return *_M_iter;
    }

    constexpr __basic_truncating_iterator& operator++()
    {
        if (_M_remaining > 0)
        {
            --_M_remaining;
            ++_M_iter;
        }

        _M_total++;
        return *this;
    }

    constexpr __basic_truncating_iterator operator++(int)
    {
        auto __tmp = *this;
        ++*this;
        return __tmp;
    }

    constexpr _Out __iter() const noexcept
    {
        return _M_iter;
    }

    constexpr std::size_t __total() const noexcept
    {
        return _M_total;
    }

private:

    _Out                _M_iter;
    std::size_t         _M_remaining = 0;
    std::size_t         _M_total = 0;
    mutable value_type  _M_dummy;
};

template <class _Out> using __truncating_iterator = __basic_truncating_iterator<_Out, char>;
template <class _Out> using __truncating_witerator = __basic_truncating_iterator<_Out, wchar_t>;


template <class _CharT>
class __basic_discarding_iterator
{
public:

    using value_type        = _CharT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = _CharT*;
    using reference         = _CharT&;
    using iterator_category = std::output_iterator_tag;

    constexpr __basic_discarding_iterator() = default;

    constexpr reference operator*() const
    {
        return _M_dummy;
    }

    constexpr __basic_discarding_iterator& operator++()
    {
        ++_M_count;
        return this;
    }

    constexpr __basic_discarding_iterator operator++(int)
    {
        auto __tmp = *this;
        ++*this;
        return __tmp;
    }

    constexpr std::size_t __count() const noexcept
    {
        return _M_count;
    }

private:

    std::size_t     _M_count = 0;
    mutable _CharT  _M_dummy;
};

using __discarding_iterator = __basic_discarding_iterator<char>;
using __discarding_witerator = __basic_discarding_iterator<wchar_t>;


template <class _Out, class _CharT>
struct __format_iterator_type 
{
    using __type = _Out;
};

template <class _Container, class _CharT>
    requires contiguous_iterator<typename _Container::iterator>
             && requires(_Container& __c)
             {
                 { __c.data() } -> same_as<_CharT*>;
             }
struct __format_iterator_type<std::back_insert_iterator<_Container>, _CharT>
{
    using __type = __basic_contiguous_append_iterator<_CharT>;
};

template <class _Out, class _CharT>
using __format_iterator_t = typename __format_iterator_type<_Out, _CharT>::__type;


} // namespace __detail


using format_parse_context = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

using format_context = basic_format_context<__detail::__contiguous_append_iterator, char>;
using wformat_context = basic_format_context<__detail::__contiguous_wappend_iterator, wchar_t>;

using format_args = basic_format_args<format_context>;
using wformat_args = basic_format_args<wformat_context>;

template <class _Out, class _CharT>
using format_args_t = basic_format_args<basic_format_context<_Out, _CharT>>;


// Declared in a "non-enabled" manner; must be specialised for each type.
template <class _T, class _CharT> struct formatter
{
    formatter() = delete;
    formatter(const formatter&) = delete;
    formatter(formatter&&) = delete;

    formatter& operator=(const formatter&) = delete;
    formatter& operator=(formatter&&) = delete;
};


namespace __detail
{

//! @TODO: implement.
template <class F, class _T, class _CharT>
concept __is_formatter_for = true;


} // namespace __detail


class format_error :
    public __XVI_STD_UTILITY_NS::runtime_error
{
public:

    explicit format_error(const string& __what_arg) :
        runtime_error(__what_arg)
    {
    }

    explicit format_error(const char* __what_arg) :
        runtime_error(__what_arg)
    {
    }
};


template <class _CharT>
class basic_format_parse_context
{
public:

    using char_type         = _CharT;
    using const_iterator    = typename basic_string_view<_CharT>::const_iterator;
    using iterator          = const_iterator;

private:

    enum __indexing { __unknown, __manual, __automatic };

    iterator    _M_begin = {};
    iterator    _M_end = {};
    __indexing  _M_indexing = __unknown;
    size_t      _M_next_arg_id = 0;
    size_t      _M_num_args = 0;

public:

    constexpr explicit basic_format_parse_context(basic_string_view<_CharT> __fmt, size_t __num_args = 0) noexcept :
        _M_begin(__fmt.begin()),
        _M_end(__fmt.end()),
        _M_indexing(__unknown),
        _M_next_arg_id(0),
        _M_num_args(__num_args)
    {
    }

    basic_format_parse_context(const basic_format_parse_context&) = delete;
    basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

    constexpr const_iterator begin() const noexcept
    {
        return _M_begin;
    }

    constexpr const_iterator end() const noexcept
    {
        return _M_end;
    }

    constexpr void advance_to(const_iterator __it)
    {
        _M_begin = __it;
    }

    constexpr size_t next_arg_id()
    {
        if (_M_indexing == __manual)
            throw format_error("inconsistent format indexing");

        _M_indexing = __automatic;

        if (_M_next_arg_id >= _M_num_args)
            throw format_error("insufficient arguments");        

        return _M_next_arg_id++;
    }

    constexpr void check_arg_id(size_t __id)
    {
        if (_M_indexing == __automatic)
            throw format_error("inconsistent format indexing");

        if (__id >= _M_num_args)
            throw format_error("invalid format argument number");

        _M_indexing = __manual;
    }
};


namespace __detail
{

template <class _Context, class... _Args>
struct __format_arg_store
{
    array<basic_format_arg<_Context>, sizeof...(_Args)> __args;
};

} // namespace __detail


template <class _Context>
class basic_format_arg
{
public:

    class handle
    {
    private:

        using _CharT = typename _Context::char_type;
    
        const void* _M_handle = nullptr;
        void (*_M_format)(basic_format_parse_context<_CharT>&, _Context&, const void*);

        template <class _T>
        explicit handle(const _T& __val) noexcept :
            _M_handle{addressof(__val)},
            _M_format{[](basic_format_parse_context<_CharT>& __parse, _Context& __ctx, const void* __ptr)
                {
                    typename _Context::template formatter_type<_T> __f;
                    __parse.advance_to(__f.parse(__parse));
                    __ctx.advance_to(__f.format(*static_cast<const _T*>(__ptr), __ctx));
                }}
        {
        }

        friend class basic_format_arg<_Context>;

    public:

        void format(basic_format_parse_context<_CharT>& __parse, _Context& __ctx) const
        {
            _M_format(__parse, __ctx, _M_handle);
        }
    };

private:

    using _CharT = typename _Context::char_type;

    using _Variant = variant<monostate,
                            bool,
                            _CharT,
                            int,
                            unsigned int,
                            long long int,
                            unsigned long long int,
                            float,
                            double,
                            long double,
                            const _CharT*,
                            basic_string_view<_CharT>,
                            const void*,
                            handle>;

    _Variant _M_value = {};

    template <class _T>
    explicit basic_format_arg(const _T& __v) noexcept
        requires __detail::__is_formatter_for<typename _Context::template formatter_type<_T>, _T, _CharT>
        && requires
        {
            { typename _Context::template formatter_type<_T>().format(declval<const _T&>(), declval<_Context&>()) };
        }
    {
        if constexpr (is_same_v<_T, bool> || is_same_v<_T, _CharT>)
            _M_value = __v;
        else if constexpr (is_same_v<_T, char> && is_same_v<_CharT, wchar_t>)
            _M_value = static_cast<wchar_t>(__v);
        else if constexpr (is_signed_v<_T> && sizeof(_T) <= sizeof(int))
            _M_value = static_cast<int>(__v);
        else if constexpr (is_unsigned_v<_T> && sizeof(_T) <= sizeof(unsigned int))
            _M_value = static_cast<unsigned int>(__v);
        else if constexpr (is_signed_v<_T> && sizeof(_T) <= sizeof(long long int))
            _M_value = static_cast<long long int>(__v);
        else if constexpr (is_unsigned_v<_T> && sizeof(_T) <= sizeof(unsigned long long int))
            _M_value = static_cast<unsigned long long int>(__v);
        else
            _M_value = handle(__v);
    }

    explicit basic_format_arg(float __n) noexcept :
        _M_value{__n}
    {
    }

    explicit basic_format_arg(double __n) noexcept :
        _M_value{__n}
    {
    }

    explicit basic_format_arg(long double __n) noexcept :
        _M_value{__n}
    {
    }

    explicit basic_format_arg(const _CharT* __s) :
        _M_value{__s}
    {
    }

    template <class _Traits>
    explicit basic_format_arg(basic_string_view<_CharT, _Traits> __sv) noexcept :
        _M_value{__sv}
    {
    }

    template <class _Traits, class _Allocator>
    explicit basic_format_arg(const basic_string<_CharT, _Traits, _Allocator>& __s) noexcept :
        _M_value{basic_string_view<_CharT>{__s.data(), __s.size()}}
    {
    }

    explicit basic_format_arg(nullptr_t) noexcept :
        _M_value{static_cast<const void*>(nullptr)}
    {
    }

    template <class _T>
    explicit basic_format_arg(const _T* __p) noexcept
        requires is_void_v<_T>
        : _M_value{__p}
    {
    }

    template <class _Visitor>
    friend decltype(auto) visit_format_arg(_Visitor&& __vis, basic_format_arg<_Context> __arg)
    {
        return visit(std::forward<_Visitor>(__vis), __arg._M_value);
    }

    template <class _Ctx, class... _Args> friend auto make_format_args(const _Args&...);

public:

    basic_format_arg() noexcept :
        _M_value{} // holds monostate
    {
    }

    basic_format_arg(const basic_format_arg&) noexcept = default;
    basic_format_arg& operator=(const basic_format_arg&) noexcept = default;
    ~basic_format_arg() = default;

    explicit operator bool() const noexcept
    {
        return !holds_alternative<monostate>(_M_value);
    }
};


template <class _Context>
class basic_format_args
{
private:

    template <class _C> friend class basic_format_args;

    size_t                              _M_size = 0;
    const basic_format_arg<_Context>*   _M_data = nullptr;

public:

    basic_format_args() noexcept = default;

    template <class... _Args>
    basic_format_args(const __detail::__format_arg_store<_Context, _Args...>& __store) noexcept :
        _M_size{sizeof...(_Args)},
        _M_data{__store.__args.data()}
    {
    }

    basic_format_arg<_Context> get(size_t __i) const noexcept
    {
        return (__i < _M_size) ? _M_data[__i] : basic_format_arg<_Context>();
    }

    std::size_t __nargs() const noexcept
    {
        return _M_size;
    }
};


template <class _Out, class _CharT>
class basic_format_context
{
private:

    basic_format_args<basic_format_context>     _M_args;
    _Out                                        _M_out;
    optional<__XVI_STD_LOCALE_NS::locale>       _M_locale;

    basic_format_context(const _Out& __out, const basic_format_args<basic_format_context>& __args) :
        _M_args(__args),
        _M_out(__out),
        _M_locale(std::nullopt)
    {
    }

    basic_format_context(const locale& __loc, const _Out& __out, const basic_format_args<basic_format_context>& __args) :
        _M_args(__args),
        _M_out(__out),
        _M_locale(__loc)
    {
    }

public:

    using char_type     = _CharT;
    using iterator      = _Out;

    template <class _T> using formatter_type = formatter<_T, _CharT>;

    basic_format_arg<basic_format_context> arg(size_t __id) const
    {
        return _M_args.get(__id);
    }

    __XVI_STD_LOCALE_NS::locale locale()
    {
        if (_M_locale)
            return *_M_locale;
        else
            return __XVI_STD_LOCALE_NS::locale();
    }

    iterator out()
    {
        return _M_out;
    }

    void advance_to(iterator __it)
    {
        _M_out = __it;
    }

    std::size_t __nargs() const noexcept
    {
        return _M_args.__nargs();
    }


    static constexpr basic_format_context __make(const _Out& __out, const basic_format_args<basic_format_context>& __args)
    {
        return {__out, __args};
    }

    static constexpr basic_format_context __make(const std::locale& __loc, const _Out& __out, const basic_format_args<basic_format_context>& __args)
    {
        return {__loc, __out, __args};
    }
};


template <class _Out>
struct format_to_n_result
{
    _Out                    out;
    iter_difference_t<_Out> size;
};


template <class _Context = format_context, class... _Args>
auto make_format_args(const _Args&... __args)
{
    using __char_t = typename _Context::char_type;
    using __iter_t = __detail::__format_iterator_t<typename _Context::iterator, __char_t>;
    using __context_t = basic_format_context<__iter_t, __char_t>;

    return __detail::__format_arg_store<__context_t, _Args...>{basic_format_arg<_Context>(__args)...};
}

template <class... _Args>
__detail::__format_arg_store<wformat_context, _Args...> make_wformat_args(const _Args&... __args)
{
    return make_format_args<wformat_context, _Args...>(__args...);
}


namespace __detail
{

template <class _CharT> extern basic_string_view<_CharT> __format_replace_chars;
template <> constexpr string_view __format_replace_chars<char>{"{}"};
template <> constexpr wstring_view __format_replace_chars<wchar_t>{L"{}"};

template <class _Out, class _CharT>
void __do_vformat(basic_string_view<_CharT> __fmt, basic_format_context<_Out, _CharT>& __ctxt)
{
    using __format_context  = basic_format_context<_Out, _CharT>;
    using __parse_context   = basic_format_parse_context<_CharT>;
    using __format_arg      = basic_format_arg<__format_context>;
    using __string_view     = basic_string_view<_CharT>;

    __parse_context __pc(__fmt, __ctxt.__nargs());
    _Out __out = __ctxt.out();

    while (true)
    {
        // Find the next replacement field in the input.
        auto __offset = __fmt.find_first_of(__format_replace_chars<_CharT>);

        // No more replacements left?
        if (__offset == __string_view::npos)
        {
            ranges::copy(__fmt, __out);
            break;
        }

        // Append the format string contents before the replacement field then trim them.
        __out = std::copy(__fmt.begin(), __fmt.begin() + __offset, __out);
        __fmt.remove_prefix(__offset);

        // If we don't have at least one further character, the format string is malformed.
        if (__fmt.length() < 2)
            throw format_error("malformed format string");

        // Is this an escape sequence?
        if (__fmt[0] == __fmt[1])
        {
            *__out++ = __fmt[0];
            __fmt.remove_prefix(2);
            continue;
        }

        // If this isn't the start of a replacement sequence, the format string is malformed.
        if (__fmt[0] != _CharT('{'))
            throw format_error("unexpected \'}\' in format string");

        // Skip the beginning of the replacement sequence.
        __fmt.remove_prefix(1);

        // Parse an optional argument ID.
        std::optional<std::size_t> __parsed_arg_id = std::nullopt;
        if (auto __c = __fmt[0]; _CharT('0') <= __c && __c <= _CharT('9'))
        {   
            if (__c == _CharT('0'))
            {
                __parsed_arg_id = 0U;
                __fmt.remove_prefix(1);
            }
            else
            {
                __parsed_arg_id = 0U;
                while (_CharT('0') <= __c && __c <= _CharT('9'))
                {
                    __parsed_arg_id = (*__parsed_arg_id * 10) + static_cast<std::size_t>(__c - _CharT('0'));
                    __fmt.remove_prefix(1);

                    if (__fmt.length() == 0)
                        throw format_error("malformed format string");

                    __c = __fmt[0];
                }
            }
        }

        if (__fmt.length() == 0)
            throw format_error("malformed format string");

        // Validate the argument index.
        std::size_t __arg_id;
        if (__parsed_arg_id)
        {
            // Throws if not valid.
            __pc.check_arg_id(*__parsed_arg_id);
            __arg_id = *__parsed_arg_id;
        }
        else
        {
            __arg_id = __pc.next_arg_id();
        }

        // The next character should be ':' to introduce a format specifier or '}' to end the replacement.
        if (__fmt[0] == _CharT(':'))
            __fmt.remove_prefix(1);

        // Ensure the parse and format contexts is up-to-date.
        __pc.advance_to(__fmt.data());
        __ctxt.advance_to(__out);

        // Get the requested argument and format it. This will parse the format string for any type-specific
        // formatting specifiers and update the parse context accordingly.
        auto __arg = __ctxt.arg(__arg_id);

        if (!__arg)
            throw format_error("missing format argument");

        visit_format_arg([&__pc, &__ctxt]<class _A>(_A&& __a) -> void
        {
            using __type = std::remove_cvref_t<_A>;

            if constexpr (is_same_v<__type, std::monostate>)
            {
                // Won't happen due to !__arg check above.
            }
            else if constexpr (is_same_v<__type, typename __format_arg::handle>)
            {
                __a.format(__pc, __ctxt);
            }
            else
            {
                using __formatter = typename __format_context::template formatter_type<__type>;
                
                __formatter __f;
                __pc.advance_to(__f.parse(__pc));
                __ctxt.advance_to(__f.format(__a, __ctxt));
            }
        }, __arg);

        // Skip the replacement characters consumed by the formatter.
        __fmt = {__pc.begin(), static_cast<std::size_t>(__pc.end() - __pc.begin())};
        __out = __ctxt.out();

        // Anything other than the end of the replacement is an error.
        if (__fmt[0] != _CharT('}'))
            throw format_error("malformed format string");
        __fmt.remove_prefix(1);

        if (__fmt.empty())
            break;
    }
}

#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
extern template __SYSTEM_CXX_FORMAT_EXPORT void __do_vformat<__contiguous_append_iterator, char>(string_view, format_context&);
extern template __SYSTEM_CXX_FORMAT_EXPORT void __do_vformat(wstring_view, wformat_context&);
#endif

} // namespace __detail


__SYSTEM_CXX_FORMAT_EXPORT string vformat(string_view __fmt, format_args __args);
__SYSTEM_CXX_FORMAT_EXPORT wstring vformat(wstring_view __fmt, wformat_args __args);
__SYSTEM_CXX_FORMAT_EXPORT string vformat(const locale& __loc, string_view __fmt, format_args __args);
__SYSTEM_CXX_FORMAT_EXPORT wstring vformat(const locale& __loc, wstring_view __fmt, wformat_args __args);


template <class... _Args>
string format(string_view __fmt, const _Args&... __args)
{
    return vformat(__fmt, make_format_args(__args...));
}

template <class... _Args>
wstring format(wstring_view __fmt, const _Args&... __args)
{
    return vformat(__fmt, make_wformat_args(__args...));
}

template <class... _Args>
string format(const locale& __loc, string_view __fmt, const _Args&... __args)
{
    return vformat(__loc, __fmt, make_format_args(__args...));
}

template <class... _Args>
wstring format(const locale& __loc, wstring_view __fmt, const _Args&... __args)
{
    return vformat(__loc, __fmt, make_wformat_args(__args...));
}


template <class _Out>
    requires output_iterator<_Out, const char&>
_Out vformat_to(_Out __out, string_view __fmt, format_args_t<type_identity_t<_Out>, char> __args)
{
    auto __ctxt = format_context::__make(__out, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __ctxt.out();
}

template <class _Out>
    requires output_iterator<_Out, const wchar_t&>
_Out vformat_to(_Out __out, wstring_view __fmt, format_args_t<type_identity_t<_Out>, wchar_t> __args)
{
    auto __ctxt = format_context::__make(__out, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __ctxt.out();
}

template <class _Out>
    requires output_iterator<_Out, const char&>
_Out vformat_to(_Out __out, const locale& __loc, string_view __fmt, format_args_t<type_identity_t<_Out>, char> __args)
{
    auto __ctxt = format_context::__make(__loc, __out, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __ctxt.out();
}

template <class _Out>
    requires output_iterator<_Out, const wchar_t&>
_Out vformat_to(_Out __out, const locale& __loc, wstring_view __fmt, format_args_t<type_identity_t<_Out>, wchar_t> __args)
{
    auto __ctxt = format_context::__make(__loc, __out, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __ctxt.out();
}



template <class _Out, class... _Args>
_Out format_to(_Out __out, string_view __fmt, const _Args&... __args)
{
    using _Context = basic_format_context<_Out, decltype(__fmt)::value_type>;
    return vformat_to(__out, __fmt, {make_format_args<_Context>(__args...)});
}

template <class _Out, class... _Args>
_Out format_to(_Out __out, wstring_view __fmt, const _Args&... __args)
{
    using _Context = basic_format_context<_Out, decltype(__fmt)::value_type>;
    return vformat_to(__out, __fmt, {make_format_args<_Context>(__args...)});
}

template <class _Out, class... _Args>
_Out format_to(_Out __out, const locale& __loc, string_view __fmt, const _Args&... __args)
{
    using _Context = basic_format_context<_Out, decltype(__fmt)::value_type>;
    return vformat_to(__out, __loc, __fmt, {make_format_args<_Context>(__args...)});
}

template <class _Out, class... _Args>
_Out format_to(_Out __out, const locale& __loc, wstring_view __fmt, const _Args&... __args)
{
    using _Context = basic_format_context<_Out, decltype(__fmt)::value_type>;
    return vformat_to(__out, __loc, __fmt, {make_format_args<_Context>(__args...)});
}


template <class _Out, class... _Args>
    requires output_iterator<_Out, const char&>
format_to_n_result<_Out> format_to_n(_Out __out, iter_difference_t<_Out> __n, string_view __fmt, const _Args&... __args)
{
    using __iter_t = __detail::__truncating_iterator<_Out>;
    using __context_t = basic_format_context<__iter_t, char>;

    __iter_t __iter(__out);
    auto __ctxt = __context_t::__make(__iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return {__iter.__iter(), __iter.total()};
}

template <class _Out, class... _Args>
    requires output_iterator<_Out, const wchar_t&>
format_to_n_result<_Out> format_to_n(_Out __out, iter_difference_t<_Out> __n, wstring_view __fmt, const _Args&... __args)
{
    using __iter_t = __detail::__truncating_witerator<_Out>;
    using __context_t = basic_format_context<__iter_t, wchar_t>;

    __iter_t __iter(__out);
    auto __ctxt = __context_t::__make(__iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return {__iter.__iter(), __iter.total()};
}

template <class _Out, class... _Args>
    requires output_iterator<_Out, const char&>
format_to_n_result<_Out> format_to_n(_Out __out, iter_difference_t<_Out> __n, const locale& __loc, string_view __fmt, const _Args&... __args)
{
    using __iter_t = __detail::__truncating_iterator<_Out>;
    using __context_t = basic_format_context<__iter_t, char>;

    __iter_t __iter(__out);
    auto __ctxt = __context_t::__make(__loc, __iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return {__iter.__iter(), __iter.total()};
}

template <class _Out, class... _Args>
    requires output_iterator<_Out, const wchar_t&>
format_to_n_result<_Out> format_to_n(_Out __out, iter_difference_t<_Out> __n, const locale& __loc, wstring_view __fmt, const _Args&... __args)
{
    using __iter_t = __detail::__truncating_witerator<_Out>;
    using __context_t = basic_format_context<__iter_t, wchar_t>;

    __iter_t __iter(__out);
    auto __ctxt = __context_t::__make(__loc, __iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return {__iter.__iter(), __iter.total()};
}


template <class... _Args>
size_t formatted_size(string_view __fmt, const _Args&... __args)
{
    using __context_t = basic_format_context<__detail::__discarding_iterator, char>;

    __detail::__discarding_iterator __iter;
    auto __ctxt = __context_t::__make(__iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return __iter.__count();
}

template <class... _Args>
size_t formatted_size(wstring_view __fmt, const _Args&... __args)
{
    using __context_t = basic_format_context<__detail::__discarding_witerator, char>;

    __detail::__discarding_witerator __iter;
    auto __ctxt = __context_t::__make(__iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return __iter.__count();
}

template <class... _Args>
size_t formatted_size(const locale& __loc, string_view __fmt, const _Args&... __args)
{
    using __context_t = basic_format_context<__detail::__discarding_iterator, char>;

    __detail::__discarding_iterator __iter;
    auto __ctxt = __context_t::__make(__loc, __iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return __iter.__count();
}

template <class... _Args>
size_t formatted_size(const locale& __loc, wstring_view __fmt, const _Args&... __args)
{
    using __context_t = basic_format_context<__detail::__discarding_witerator, char>;

    __detail::__discarding_witerator __iter;
    auto __ctxt = __context_t::__make(__loc, __iter, {make_format_args<__context_t>(__args...)});
    __iter = __detail::__do_vformat(__fmt, __ctxt);

    return __iter.__count();
}



namespace __detail
{


class __library_formatter_common
{
protected:

    template <class _Out, class _CharT>
    std::size_t __get_width_arg(const basic_format_arg<basic_format_context<_Out, _CharT>>& __arg)
    {
        std::size_t __width;
        visit_format_arg([&__width]<class _A>(_A&& __a)
        {
            using __type = remove_cvref_t<_A>;
            if constexpr (!is_same_v<__type, bool> && is_integral_v<__type>)
            {
                if (__a < 1)
                    throw format_error("width argument must be positive");
                __width = static_cast<std::size_t>(__a);
            }
            else
                throw format_error("width argument must be an integral type");
        }, __arg);

        return __width;
    }

    template <class _Out, class _CharT>
    std::size_t __get_precision_arg(const basic_format_arg<basic_format_context<_Out, _CharT>>& __arg)
    {
        std::size_t __precision;
        visit_format_arg([&__precision]<class _A>(_A&& __a)
        {
            using __type = remove_cvref_t<_A>;
            if constexpr (!is_same_v<__type, bool> && is_integral_v<__type>)
            {
                if (__a < 0)
                    throw format_error("precision argument must be non-negative");
                __precision = static_cast<std::size_t>(__a);
            }
            else
                throw format_error("precision argument must be an integral type");
        }, __arg);

        return __precision;
    }
};


template <class _CharT>
class __library_formatter_base :
    protected __library_formatter_common
{
protected:

    using __library_formatter_common::__get_width_arg;
    using __library_formatter_common::__get_precision_arg;

    using __parse_ctxt = basic_format_parse_context<_CharT>;

    // Indices match the order of the characters in __format_alignment_chars.
    enum class __alignment : std::int8_t {  __default = -1, __left, __centre, __right, };

    // Indices match the order of the characters in __format_sign_chars.
    enum class __sign_type : std::int8_t { __default = -1, __always, __negatives, __padded, };

    // Indices match the order of the characters in __format_presentation_chars.
    enum class __presentation_type : std::int8_t { __default = -1, __a, __A, __b, __B, __c, __d, __e, __E, __f, __F, __g, __G, __o, __p, __s, __x, __X, };

    enum class __width_precision_type : std::int8_t { __default = -1, __value, __arg, __numbered_arg, };


    _CharT                      _M_fill;
    __alignment                 _M_align;
    __sign_type                 _M_sign;
    bool                        _M_alt_form;
    bool                        _M_zero_pad;
    bool                        _M_locale_specific;
    __presentation_type         _M_presentation;
    __width_precision_type      _M_width_type;
    __width_precision_type      _M_precision_type;
    std::size_t                 _M_width_data;
    std::size_t                 _M_precision_data;


    // Defined in Format.cc
    __SYSTEM_CXX_FORMAT_EXPORT
    typename __parse_ctxt::iterator __parse_base(__parse_ctxt&);
};

template <class _T, class _CharT>
class __library_formatter :
    private __library_formatter_base<_CharT>
{
public:

    using __library_formatter_common::__get_width_arg;  
    using __library_formatter_common::__get_precision_arg;

    template <class _Out>
    using __format_ctxt = basic_format_context<_Out, _CharT>;
    using __parse_ctxt = basic_format_parse_context<_CharT>;

    constexpr __library_formatter() = default;
    constexpr __library_formatter(const __library_formatter&) = default;
    constexpr __library_formatter& operator=(const __library_formatter&) = default;
    constexpr ~__library_formatter() = default;

    // Defined in Format.cc
    __SYSTEM_CXX_FORMAT_EXPORT
    typename __parse_ctxt::iterator parse(__parse_ctxt&);

    template <class _Out>
    __SYSTEM_CXX_FORMAT_EXPORT
    typename __format_ctxt<_Out>::iterator format(_T, __format_ctxt<_Out>&);

protected:

    using __base = __library_formatter_base<_CharT>;

    using __alignment               = __base::__alignment;
    using __sign_type               = __base::__sign_type;
    using __presentation_type       = __base::__presentation_type;
    using __width_precision_type    = __base::__width_precision_type;

    using __base::_M_fill;
    using __base::_M_align;
    using __base::_M_sign;
    using __base::_M_alt_form;
    using __base::_M_zero_pad;
    using __base::_M_locale_specific;
    using __base::_M_presentation;
    using __base::_M_width_type;
    using __base::_M_precision_type;
    using __base::_M_width_data;
    using __base::_M_precision_data;

    // Is this a string type?
    static constexpr bool __string_type = 
        is_same_v<_T, _CharT*>
        || is_same_v<_T, const _CharT*>
        || (is_array_v<_T> && is_same_v<remove_extent_t<_T>, _CharT>)
        || is_same_v<_T, basic_string_view<_CharT>>
        || ranges::__detail::__is_specialization_of_v<_T, basic_string>;

};


template <class _CharT> extern basic_string_view<_CharT> __format_alignment_chars;
template <> constexpr string_view __format_alignment_chars<char>{"<^>"};
template <> constexpr wstring_view __format_alignment_chars<wchar_t>{L"<^>"};

template <class _CharT> extern basic_string_view<_CharT> __format_sign_chars;
template <> constexpr string_view __format_sign_chars<char>{"+- "};
template <> constexpr wstring_view __format_sign_chars<wchar_t>{L"+- "};

template <class _CharT> extern basic_string_view<_CharT> __format_presentation_chars;
template <> constexpr string_view __format_presentation_chars<char>{"aAbBcdeEfFgGopsxX"};
template <> constexpr wstring_view __format_presentation_chars<wchar_t>{L"aAbBcdeEfFgGopsxX"};

template <class _CharT> extern basic_string_view<_CharT> __format_true_string;
template <class _CharT> extern basic_string_view<_CharT> __format_false_string;
template <> constexpr string_view __format_true_string<char>{"true"};
template <> constexpr string_view __format_false_string<char>{"false"};
template <> constexpr wstring_view __format_true_string<wchar_t>{L"true"};
template <> constexpr wstring_view __format_false_string<wchar_t>{L"false"};


template <class _T>
struct __format_unsigned { using __type = std::make_unsigned_t<_T>; };

template <>
struct __format_unsigned<bool> { using __type = unsigned char; };

template <class _T>
using __format_unsigned_t = typename __format_unsigned<_T>::__type;


template <class _T, class _CharT>
template <class _Out>
typename __library_formatter<_T, _CharT>::template __format_ctxt<_Out>::iterator __library_formatter<_T, _CharT>::format(_T __t, __format_ctxt<_Out>& __fc)
{
    auto __out = __fc.out();

    // Temporary buffer (always narrow characters).
    //
    // Max size is for binary-formatted 64-bit value.
    constexpr auto __buffer_max_size = 
        (is_arithmetic_v<_T>) ? (8 * sizeof(_T)) :
        (is_pointer_v<_T> || is_same_v<_T, nullptr_t>) ? (8 * sizeof(std::uintptr_t))
        : 1;
    std::array<char, __buffer_max_size> __buffer;
    std::array<char, 2> __prefix;
    char* __ptr = __buffer.begin();
    char* __prefix_ptr = __prefix.begin();

    bool __negative = false;
    bool __integer_presentation = false;
    std::size_t __formatted_size = 0;

    bool __has_width = false;
    bool __has_precision = false;
    std::size_t __width = 0;
    std::size_t __precision = 0;

    switch (_M_width_type)
    {
        case __width_precision_type::__value:
            __has_width = true;
            __width = _M_width_data;
            break;

        case __width_precision_type::__arg:
        case __width_precision_type::__numbered_arg: 
            __has_width = true;
            __width = __get_width_arg(__fc.arg(_M_width_data));
            break;

        default:
            break;
    };

    switch (_M_precision_type)
    {
        case __width_precision_type::__value:
            __has_precision = true;
            __precision = _M_precision_data;
            break;

        case __width_precision_type::__arg:
        case __width_precision_type::__numbered_arg:
            __has_precision = true;
            __precision = __get_precision_arg(__fc.arg(_M_precision_data));
            break;

        default:
            break;
    }

    switch (_M_presentation)
    {
        case __presentation_type::__a:
        case __presentation_type::__A:
            //! @TODO: implement.
            break;

        case __presentation_type::__e:
        case __presentation_type::__E:
            //! @TODO: implement.
            break;

        case __presentation_type::__f:
        case __presentation_type::__F:
            //! @TODO: implement.
            break;

        case __presentation_type::__g:
        case __presentation_type::__G:
        case __presentation_type::__default:
            //! @TODO: implement.
            break;

        case __presentation_type::__b:
        case __presentation_type::__B:
            if constexpr (is_integral_v<_T>)
            {
                __integer_presentation = true;
                if constexpr (is_signed_v<_T>)
                    __negative = (__t < 0);

                if (__negative)
                    __t = -__t;

                if (_M_alt_form)
                {
                    *__prefix_ptr++ = '0';
                    *__prefix_ptr++ = _M_presentation == __presentation_type::__b ? 'b' : 'B';
                }
                __ptr = std::to_chars(__ptr, __buffer.end(), static_cast<__format_unsigned_t<_T>>(__t), 2).ptr;
                __formatted_size = static_cast<std::size_t>(__ptr - __buffer.begin());
            }
            break;

        case __presentation_type::__d:
            if constexpr (is_integral_v<_T>)
            {
                __integer_presentation = true;
                if constexpr (is_signed_v<_T>)
                    __negative = (__t < 0);

                if (__negative)
                    __t = -__t;

                __ptr = std::to_chars(__ptr, __buffer.end(), static_cast<__format_unsigned_t<_T>>(__t)).ptr;
                __formatted_size = static_cast<std::size_t>(__ptr - __buffer.begin());
            }
            break;

        case __presentation_type::__o:
            if constexpr (is_integral_v<_T>)
            {
                __integer_presentation = true;
                if constexpr (is_signed_v<_T>)
                    __negative = (__t < 0);

                if (__negative)
                    __t = -__t;

                if (_M_alt_form && __t != 0)
                    *__ptr++ = '0';
                __ptr = std::to_chars(__ptr, __buffer.end(), static_cast<__format_unsigned_t<_T>>(__t), 8).ptr;
                __formatted_size = static_cast<std::size_t>(__ptr - __buffer.begin());
            }
            break;

        case __presentation_type::__x:
        case __presentation_type::__X:
            if constexpr (is_integral_v<_T>)
            {
                __integer_presentation = true;
                if constexpr (is_signed_v<_T>)
                    __negative = (__t < 0);   

                if (__negative)
                    __t = -__t;            

                if (_M_alt_form)
                {
                    *__prefix_ptr++ = '0';
                    *__prefix_ptr++ = (_M_presentation == __presentation_type::__x) ? 'x' : 'X';
                }
                __ptr = std::to_chars(__ptr, __buffer.end(), static_cast<__format_unsigned_t<_T>>(__t), 16).ptr;
                __formatted_size = static_cast<std::size_t>(__ptr - __buffer.begin());
                if (_M_presentation == __presentation_type::__X)
                    for (auto& __c : span(__buffer.begin(), __ptr))
                        __c = std::toupper(__c);
            }
            break;

        case __presentation_type::__c:
            // Not written to the buffer.
            __formatted_size = 1;
            break;

        case __presentation_type::__s:
            //! @TODO: localised boolean strings.
            if constexpr (is_same_v<_T, bool>)
                __formatted_size = __t ? 4 : 5;
            else if constexpr (__string_type)
                __formatted_size = basic_string_view<_CharT>(__t).length();

            if (__has_precision)
                __formatted_size = std::min(__formatted_size, __precision);

            break;

        case __presentation_type::__p:
            if constexpr (is_pointer_v<_T> || is_same_v<_T, nullptr_t>)
            {
                *__prefix_ptr++ = '0';
                *__prefix_ptr++ = 'x';
                __ptr = std::to_chars(__ptr, __buffer.end(), reinterpret_cast<std::uintptr_t>(__t), 16).ptr;
                __formatted_size = static_cast<std::size_t>(__ptr - __buffer.begin());
            }
            break;
    }

    // Does a sign character need to be added?
    char __sign = '\0';
    if (__integer_presentation)
    {
        if (__negative)
            __sign = '-';
        else if (_M_sign == __sign_type::__always)
            __sign = '+';
        else if (_M_sign == __sign_type::__padded)
            __sign = ' ';
    }

    if (__sign)
        __formatted_size++;

    std::size_t __prefix_size = static_cast<std::size_t>(__prefix_ptr - __prefix.begin());
    if (__prefix_size > 0)
        __formatted_size += __prefix_size;

    std::size_t __pad = 0;
    std::size_t __pad_before = 0;
    std::size_t __pad_after = 0;

    if (__has_width && __width > __formatted_size)
    {
        __pad = static_cast<std::size_t>(__width - __formatted_size);
        if (!_M_zero_pad)
        {
            switch (_M_align)
            {
                case __alignment::__left:
                    __pad_after = __pad;
                    break;

                case __alignment::__centre:
                    __pad_before = (__pad / 2);
                    __pad_after = __pad - __pad_before;
                    break;

                case __alignment::__right:
                    __pad_before = __pad;
                    break;

                case __alignment::__default:
                    // Won't happen; resolved at parse time.
                    break;
            }
        }
    }

    // Output any leading padding.
    __out = std::fill_n(__out, __pad_before, _M_fill);

    // Output the sign.
    if (__sign)
        *__out++ = _CharT(__sign);

    // Output the prefix.
    __out = std::copy(__prefix.begin(), __prefix_ptr, __out);

    // Output any zero padding.
    if (_M_zero_pad)
        __out = std::fill_n(__out, __pad, _CharT('0'));

    // Output the formatted buffer.
    switch (_M_presentation)
    {
        case __presentation_type::__a:
        case __presentation_type::__A:
        case __presentation_type::__b:
        case __presentation_type::__B:
        case __presentation_type::__d:
        case __presentation_type::__e:
        case __presentation_type::__E:
        case __presentation_type::__f:
        case __presentation_type::__F:
        case __presentation_type::__g:
        case __presentation_type::__G:
        case __presentation_type::__o:
        case __presentation_type::__p:
        case __presentation_type::__x:
        case __presentation_type::__X:
        case __presentation_type::__default:
            __out = std::copy(__buffer.begin(), __ptr, __out);
            break;

        case __presentation_type::__c:
        {
            if constexpr (is_integral_v<_T>)
            {
                auto __c = static_cast<_CharT>(__t);
                if (static_cast<_T>(__c) != __t)
                    throw format_error("lossy conversion from integer to character type");

                *__out++ = __c;
            }
            break;
        }

        case __presentation_type::__s:
        {
            if constexpr (__string_type || is_same_v<_T, bool>)
            {
                basic_string_view<_CharT> __sv;

                if constexpr (is_same_v<_T, bool>)
                    //! @TODO: support for localised true/false strings.
                    __sv = (__t) ? __format_true_string<_CharT> : __format_false_string<_CharT>;
                else
                    __sv = __t;

                auto __len = __sv.length();

                if (__has_precision)
                    __len = std::min(__len, __precision);

                __out = std::copy(__sv.begin(), __sv.begin() + __len, __out);
            }
            break;
        }
    }

    // Output any trailing padding.
    __out = std::fill_n(__out, __pad_after, _M_fill);

    return __out;
}


#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
extern template class __library_formatter_base<char>;
extern template class __library_formatter_base<wchar_t>;
#endif


} // namespace __detail


// Required specialisations of formatter.
template <> struct __SYSTEM_CXX_FORMAT_EXPORT formatter<char, char> : __detail::__library_formatter<char, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<char, char>;
    extern template format_context::iterator __detail::__library_formatter<char, char>::template format(char, format_context&);
#endif
template <> struct formatter<char, wchar_t> : __detail::__library_formatter<char, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<char, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<char, wchar_t>::template format(char, wformat_context&);
#endif
template <> struct formatter<wchar_t, wchar_t> : __detail::__library_formatter<wchar_t, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<wchar_t, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<wchar_t, wchar_t>::template format(wchar_t, wformat_context&);
#endif

template <> struct formatter<char*, char> : __detail::__library_formatter<string_view, char> {};
template <> struct formatter<const char*, char> : __detail::__library_formatter<string_view, char> {};
template <size_t _N> struct formatter<const char[_N], char> : __detail::__library_formatter<string_view, char> {};
template <class _Traits, class _Allocator> struct formatter<basic_string<char, _Traits, _Allocator>, char> : __detail::__library_formatter<string_view, char> {};
template <class _Traits> struct formatter<basic_string_view<char, _Traits>, char> : __detail::__library_formatter<string_view, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<string_view, char>;
    extern template format_context::iterator __detail::__library_formatter<string_view, char>::template format(string_view, format_context&);
#endif

template <> struct formatter<bool, char> : __detail::__library_formatter<bool, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<bool, char>;
    extern template format_context::iterator __detail::__library_formatter<bool, char>::template format(bool, format_context&);
#endif
template <> struct formatter<signed char, char> : __detail::__library_formatter<signed char, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<signed char, char>;
    extern template format_context::iterator __detail::__library_formatter<signed char, char>::template format(signed char, format_context&);
#endif
template <> struct formatter<unsigned char, char> : __detail::__library_formatter<unsigned char, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned char, char>;
    extern template format_context::iterator __detail::__library_formatter<unsigned char, char>::template format(unsigned char, format_context&);
#endif
template <> struct formatter<short, char> : __detail::__library_formatter<short, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<short, char>;
    extern template format_context::iterator __detail::__library_formatter<short, char>::template format(short, format_context&);
#endif
template <> struct formatter<unsigned short, char> : __detail::__library_formatter<unsigned short, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned short, char>;
    extern template format_context::iterator __detail::__library_formatter<unsigned short, char>::template format(unsigned short, format_context&);
#endif
template <> struct formatter<int, char> : __detail::__library_formatter<int, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<int, char>;
    extern template format_context::iterator __detail::__library_formatter<int, char>::template format(int, format_context&);
#endif
template <> struct formatter<unsigned int, char> : __detail::__library_formatter<unsigned int, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned int, char>;
    extern template format_context::iterator __detail::__library_formatter<unsigned int, char>::template format(unsigned int, format_context&);
#endif
template <> struct formatter<long, char> : __detail::__library_formatter<long, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long, char>;
    extern template format_context::iterator __detail::__library_formatter<long, char>::template format(long, format_context&);
#endif
template <> struct formatter<unsigned long, char> : __detail::__library_formatter<unsigned long, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned long, char>;
    extern template format_context::iterator __detail::__library_formatter<unsigned long, char>::template format(unsigned long, format_context&);
#endif
template <> struct formatter<long long, char> : __detail::__library_formatter<long long, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long long, char>;
    extern template format_context::iterator __detail::__library_formatter<long long, char>::template format(long long, format_context&);
#endif
template <> struct formatter<unsigned long long, char> : __detail::__library_formatter<unsigned long long, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned long long, char>;
    extern template format_context::iterator __detail::__library_formatter<unsigned long long, char>::template format(unsigned long long, format_context&);
#endif
template <> struct formatter<float, char> : __detail::__library_formatter<float, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<float, char>;
    extern template format_context::iterator __detail::__library_formatter<float, char>::template format(float, format_context&);
#endif
template <> struct formatter<double, char> : __detail::__library_formatter<double, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<double, char>;
    extern template format_context::iterator __detail::__library_formatter<double, char>::template format(double, format_context&);
#endif
template <> struct formatter<long double, char> : __detail::__library_formatter<long double, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long double, char>;
    extern template format_context::iterator __detail::__library_formatter<long double, char>::template format(long double, format_context&);
#endif
template <> struct formatter<nullptr_t, char> : __detail::__library_formatter<nullptr_t, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<nullptr_t, char>;
    extern template format_context::iterator __detail::__library_formatter<nullptr_t, char>::template format(nullptr_t, format_context&);
#endif
template <> struct formatter<void*, char> : __detail::__library_formatter<void*, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<void*, char>;
    extern template format_context::iterator __detail::__library_formatter<void*, char>::template format(void*, format_context&);
#endif
template <> struct formatter<const void*, char> : __detail::__library_formatter<const void*, char> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<const void*, char>;
    extern template format_context::iterator __detail::__library_formatter<const void*, char>::template format(const void*, format_context&);
#endif

template <> struct formatter<wchar_t*, wchar_t> : __detail::__library_formatter<wstring_view, wchar_t> {};
template <> struct formatter<const wchar_t*, wchar_t> : __detail::__library_formatter<wstring_view, wchar_t> {};
template <size_t _N> struct formatter<const wchar_t[_N], wchar_t> : __detail::__library_formatter<wstring_view, wchar_t> {};
template <class _Traits, class _Allocator> struct formatter<basic_string<wchar_t, _Traits, _Allocator>, wchar_t> : __detail::__library_formatter<wstring_view, wchar_t> {};
template <class _Traits> struct formatter<basic_string_view<wchar_t, _Traits>, wchar_t> : __detail::__library_formatter<wstring_view, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<wstring_view, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<wstring_view, wchar_t>::template format(wstring_view, wformat_context&);
#endif

template <> struct formatter<bool, wchar_t> : __detail::__library_formatter<bool, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<bool, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<bool, wchar_t>::template format(bool, wformat_context&);
#endif
template <> struct formatter<signed char, wchar_t> : __detail::__library_formatter<signed char, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<signed char, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<signed char, wchar_t>::template format(signed char, wformat_context&);
#endif
template <> struct formatter<unsigned char, wchar_t> : __detail::__library_formatter<unsigned char, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned char, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<unsigned char, wchar_t>::template format(unsigned char, wformat_context&);
#endif
template <> struct formatter<short, wchar_t> : __detail::__library_formatter<short, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<short, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<short, wchar_t>::template format(short, wformat_context&);
#endif
template <> struct formatter<unsigned short, wchar_t> : __detail::__library_formatter<unsigned short, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned short, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<unsigned short, wchar_t>::template format(unsigned short, wformat_context&);
#endif
template <> struct formatter<int, wchar_t> : __detail::__library_formatter<int, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<int, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<int, wchar_t>::template format(int, wformat_context&);
#endif
template <> struct formatter<unsigned int, wchar_t> : __detail::__library_formatter<unsigned int, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned int, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<unsigned int, wchar_t>::template format(unsigned int, wformat_context&);
#endif
template <> struct formatter<long, wchar_t> : __detail::__library_formatter<long, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<long, wchar_t>::template format(long, wformat_context&);
#endif
template <> struct formatter<unsigned long, wchar_t> : __detail::__library_formatter<unsigned long, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned long, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<unsigned long, wchar_t>::template format(unsigned long, wformat_context&);
#endif
template <> struct formatter<long long, wchar_t> : __detail::__library_formatter<long long, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long long, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<long long, wchar_t>::template format(long long, wformat_context&);
#endif
template <> struct formatter<unsigned long long, wchar_t> : __detail::__library_formatter<unsigned long long, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<unsigned long long, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<unsigned long long, wchar_t>::template format(unsigned long long, wformat_context&);
#endif
template <> struct formatter<float, wchar_t> : __detail::__library_formatter<float, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<float, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<float, wchar_t>::template format(float, wformat_context&);
#endif
template <> struct formatter<double, wchar_t> : __detail::__library_formatter<double, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<double, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<double, wchar_t>::template format(double, wformat_context&);
#endif
template <> struct formatter<long double, wchar_t> : __detail::__library_formatter<long double, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<long double, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<long double, wchar_t>::template format(long double, wformat_context&);
#endif
template <> struct formatter<nullptr_t, wchar_t> : __detail::__library_formatter<nullptr_t, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<nullptr_t, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<nullptr_t, wchar_t>::template format(nullptr_t, wformat_context&);
#endif
template <> struct formatter<void*, wchar_t> : __detail::__library_formatter<void*, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<void*, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<void*, wchar_t>::template format(void*, wformat_context&);
#endif
template <> struct formatter<const void*, wchar_t> : __detail::__library_formatter<const void*, wchar_t> {};
#if !defined(__XVI_STD_FORMAT_HEADER_ONLY)
    extern template class __detail::__library_formatter<const void*, wchar_t>;
    extern template wformat_context::iterator __detail::__library_formatter<const void*, wchar_t>::template format(const void*, wformat_context&);
#endif


} // namespace __XVI_STD_FORMAT_NS


#endif /* ifndef __SYSTEM_CXX_FORMAT_FORMAT_H */

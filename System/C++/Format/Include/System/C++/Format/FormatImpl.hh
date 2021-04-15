#ifndef __SYSTEM_CXX_FORMAT_FORMATIMPL_H
#define __SYSTEM_CXX_FORMAT_FORMATIMPL_H

#include <System/C++/Format/Format.hh>

#include <System/C++/Utility/Optional.hh>


namespace __XVI_STD_FORMAT_NS
{


namespace __detail
{



template <class _CharT>
typename __library_formatter_base<_CharT>::__parse_ctxt::iterator __library_formatter_base<_CharT>::__parse_base(__parse_ctxt& __pc)
{
    using __sv = basic_string_view<_CharT>;

    __sv __fmt{__pc.begin(), static_cast<std::size_t>(__pc.end() - __pc.begin())};

    // Read the fill and alignment.
    _M_fill = _CharT(' ');
    _M_align = __alignment::__default;
    if (__fmt.length() >= 2)
    {
        auto __a = __format_alignment_chars<_CharT>.find(__fmt[1]);
        if (__a != __sv::npos)
        {
            _M_align = static_cast<__alignment>(__a);
            _M_fill = __fmt[0];
            __fmt.remove_prefix(2);
        }
    }
    if (__fmt.length() >= 1 && _M_align == __alignment::__default)
    {
        auto __a = __format_alignment_chars<_CharT>.find(__fmt[0]);
        if (__a != __sv::npos)
        {
            _M_align = static_cast<__alignment>(__a);
            __fmt.remove_prefix(1);
        }
    }

    // Read the sign specifier.
    _M_sign = __sign_type::__default;
    if (__fmt.length() >= 1)
    {
        auto __s = __format_sign_chars<_CharT>.find(__fmt[0]);
        if (__s != __sv::npos)
        {
            _M_sign = static_cast<__sign_type>(__s);
            __fmt.remove_prefix(1);
        }
    }

    // Read the alt-form and zero-pad formatting flags.
    _M_alt_form = false;
    _M_zero_pad = false;
    if (__fmt.length() >= 1 && __fmt[0] == _CharT('#'))
    {
        _M_alt_form = true;
        __fmt.remove_prefix(1);
    }
    if (__fmt.length() >= 1 && __fmt[0] == _CharT('0'))
    {
        _M_zero_pad = true;
        __fmt.remove_prefix(1); 
    }

    // Read the width field.
    // Note that it cannot begin with a zero.
    _M_width_type = __width_precision_type::__default;
    _M_width_data = 0;
    if (__fmt.length() >= 1 && __fmt[0] == _CharT('{'))
    {
        _M_width_type = __width_precision_type::__arg;
        __fmt.remove_prefix(1);
    }
    if (__fmt.length() >= 1 && _CharT('1') <= __fmt[0] && __fmt[0] <= _CharT('9'))
    {
        if (_M_width_type == __width_precision_type::__arg)
            _M_width_type = __width_precision_type::__numbered_arg;
        else
            _M_width_type = __width_precision_type::__value;

        while (__fmt.length() >= 1 && _CharT('0') <= __fmt[0] && __fmt[0] <= _CharT('9'))
        {
            _M_width_data = (_M_width_data * 10) + static_cast<std::size_t>(__fmt[0] - _CharT('0'));
            __fmt.remove_prefix(1);
        }
    }
    if (_M_width_type == __width_precision_type::__arg || _M_width_type == __width_precision_type::__numbered_arg)
    {
        if (__fmt.length() < 1 || __fmt[0] != _CharT('}'))
            throw format_error("expected \'}\' at end of argument-based width field");

        __fmt.remove_prefix(1);

        if (_M_width_type == __width_precision_type::__arg)
        {
            _M_width_data = __pc.next_arg_id();
            _M_width_type = __width_precision_type::__numbered_arg;
        }
        else
        {
            __pc.check_arg_id(_M_width_data);
        }
    }

    // Read the precision field.
    _M_precision_type = __width_precision_type::__default;
    _M_precision_data = 0;
    if (__fmt.length() >= 1 && __fmt[0] == _CharT('.'))
    {
        __fmt.remove_prefix(1);

        if (__fmt.length() >= 1 && __fmt[0] == _CharT('{'))
        {
            _M_precision_type = __width_precision_type::__arg;
            __fmt.remove_prefix(1);
        }

        if (__fmt.length() >= 1 && _CharT('0') <= __fmt[0] && __fmt[0] <= _CharT('9'))
        {
            if (_M_precision_type == __width_precision_type::__arg)
                _M_precision_type = __width_precision_type::__numbered_arg;
            else
                _M_precision_type = __width_precision_type::__value;

            while (__fmt.length() >= 1 && _CharT('0') <= __fmt[0] && __fmt[0] <= _CharT('9'))
            {
                _M_precision_data = (_M_precision_data * 10) + static_cast<std::size_t>(__fmt[0] - _CharT('0'));
                __fmt.remove_prefix(1);
            }
        }

        if (_M_precision_type == __width_precision_type::__arg || _M_precision_type == __width_precision_type::__numbered_arg)
        {
            if (__fmt.length() < 1 || __fmt[0] != '}')
                throw format_error("expected \'}\' at end of argument-based precision field");

            __fmt.remove_prefix(1);

            if (_M_precision_type == __width_precision_type::__arg)
            {
                _M_precision_data = __pc.next_arg_id();
                _M_precision_type = __width_precision_type::__numbered_arg;
            }
            else
            {
                __pc.check_arg_id(_M_precision_data);
            }
        }

        if (_M_precision_type == __width_precision_type::__default)
            throw format_error("expected precision value after \'.\'");
    }

    // Check for locale-specific formatting.
    _M_locale_specific = false;
    if (__fmt.length() >= 1 && __fmt[0] == _CharT('L'))
    {
        _M_locale_specific = true;
        __fmt.remove_prefix(1);
    }

    // Check for a presentation type.
    _M_presentation = __presentation_type::__default;
    if (__fmt.length() >= 1)
    {
        auto __p = __format_presentation_chars<_CharT>.find(__fmt[0]);
        if (__p != __sv::npos)
        {
            _M_presentation = static_cast<__presentation_type>(__p);
            __fmt.remove_prefix(1);
        }
    }

    // The only valid final states are to have reached the end of the format string or for the next char to be '}'.
    if (__fmt.length() > 0 && __fmt[0] != _CharT('}'))
        throw format_error("expected \'}\' at end of format specifier");

    return __fmt.data();
}

template <class _T, class _CharT>
typename __library_formatter<_T, _CharT>::__parse_ctxt::iterator __library_formatter<_T, _CharT>::parse(__parse_ctxt& __pc)
{
    // First, parse the format without regard to the type.
    auto __iter = __library_formatter_base<_CharT>::__parse_base(__pc);

    // Is the presentation form of an integer type?
    bool __integer_presentation = false;
    switch (_M_presentation)
    {
        case __presentation_type::__b:
        case __presentation_type::__B:
        case __presentation_type::__d:
        case __presentation_type::__o:
        case __presentation_type::__x:
        case __presentation_type::__X:
            __integer_presentation = true;
            break;

        default:
            break;
    }

    // Check that the zero-pad option is valid.
    if (_M_zero_pad)
    {
        if constexpr (is_same_v<_T, bool>)
        {
            if (!__integer_presentation)
                throw format_error("cannot request zero-pad formatting for bool with non-integer presentation");
        }
        else if constexpr (is_same_v<_T, _CharT>)
        {
            if (!__integer_presentation)
                throw format_error("cannot request zero-pad formatting for character with non-integer presentation");
        }
        else if constexpr (!is_arithmetic_v<_T>)
        {
            throw format_error("cannot request zerp-pad formatting for non-arithmetic types");
        }
    }

    // Resolve the default alignment.
    if (_M_align == __alignment::__default)
    {
        if constexpr (is_same_v<_T, bool> || is_same_v<_T, _CharT>)
            _M_align = __integer_presentation ? __alignment::__right : __alignment::__left;
        else if constexpr (is_arithmetic_v<_T>)
            _M_align = __alignment::__right;
        else
            _M_align = __alignment::__left;
    }
    else
    {
        // Explicit alignment overrides the zero-pad flag.
        _M_zero_pad = false;
    }

    // Resolve the sign style.
    if constexpr (is_same_v<_T, bool>)
    {
        if (!__integer_presentation && _M_sign != __sign_type::__default)
            throw format_error("cannot specify sign option for bool with non-integer presentation");
        else if (__integer_presentation && _M_sign == __sign_type::__default)
            _M_sign = __sign_type::__negatives;
    }
    else if constexpr (is_same_v<_T, _CharT>)
    {
        if (!__integer_presentation && _M_sign != __sign_type::__default)
            throw format_error("cannot specify sign option for character with non-integer presentation");
        else if (__integer_presentation && _M_sign == __sign_type::__default)
            _M_sign = __sign_type::__negatives;
    }
    else if constexpr (is_arithmetic_v<_T>)
    {
        if (_M_sign == __sign_type::__default)
            _M_sign = __sign_type::__negatives;
    }
    else
    {
        if (_M_sign != __sign_type::__default)
            throw format_error("cannot specify sign option for non-integer type");
    }

    // Check that the alternate form option is valid.
    if (_M_alt_form)
    {
        if constexpr (is_same_v<_T, bool>)
        {
            if (!__integer_presentation)
                throw format_error("cannot request alternate form formatting for bool with non-integer presentation");
        }
        else if constexpr (is_same_v<_T, _CharT>)
        {
            if (!__integer_presentation)
                throw format_error("cannot request alternate form formatting for character with non-integer presentation");
        }
        else if constexpr (!is_arithmetic_v<_T>)
        {
            throw format_error("cannot request alternate form for non-arithmetic types");
        }
    }

    // Precision is only valid for floating-point and string types.
    if (_M_precision_type != __width_precision_type::__default)
    {
        if constexpr (!is_floating_point_v<_T> && !__string_type)
            throw format_error("precision is only valid for floating-point and string types");
    }

    // Locale-specific formatting is only valid for arithmetic types.
    if (_M_locale_specific)
    {
        if constexpr (!is_arithmetic_v<_T>)
            throw format_error("cannot use locale-specific formatting for non-arithmetic types");
    }

    // Check that the presentation type matches the actual type.
    if constexpr (__string_type)
    {
        switch (_M_presentation)
        {
            case __presentation_type::__default:
                _M_presentation = __presentation_type::__s;
                break;

            case __presentation_type::__s:
                break;

            default:
                throw format_error("invalid presentation for string type");
        }
    }
    else if constexpr (is_integral_v<_T> || is_same_v<_T, _CharT> || is_same_v<_T, bool>)
    {
        switch (_M_presentation)
        {
            case __presentation_type::__default:
                if constexpr (is_same_v<_T, _CharT>)
                    _M_presentation = __presentation_type::__c;
                else if constexpr (is_same_v<_T, bool>)
                    _M_presentation = __presentation_type::__s;
                else
                    _M_presentation = __presentation_type::__d;
                break;

            case __presentation_type::__b:
            case __presentation_type::__B:
            case __presentation_type::__c:
            case __presentation_type::__d:
            case __presentation_type::__o:
            case __presentation_type::__x:
            case __presentation_type::__X:
                break;

            case __presentation_type::__s:
                if constexpr (is_same_v<_T, bool>)
                    break;
                [[fallthrough]];

            default:
                throw format_error("invalid presentation for integer type");
        }
    }
    else if constexpr (is_floating_point_v<_T>)
    {
        switch (_M_presentation)
        {
            case __presentation_type::__default:
            case __presentation_type::__a:
            case __presentation_type::__A:
            case __presentation_type::__e:
            case __presentation_type::__E:
            case __presentation_type::__f:
            case __presentation_type::__F:
            case __presentation_type::__g:
            case __presentation_type::__G:
                break;

            default:
                throw format_error("invalid presentation for floating-point type");
        }
    }
    else if constexpr (is_pointer_v<_T> || is_same_v<_T, std::nullptr_t>)
    {
        switch (_M_presentation)
        {
            case __presentation_type::__default:
                _M_presentation = __presentation_type::__p;
                break;

            case __presentation_type::__p:
                break;

            default:
                throw format_error("invalid presentation for pointer type");
        }
    }
    else
    {
        // The cases above should always apply...
        static_assert(!is_same_v<_T, _T>, "Invalid type for __library_formatter");
    }
    
    return __iter;
}


} // namespace __detail


inline std::string vformat(string_view __fmt, format_args __args)
{
    std::string __out;
    __detail::__contiguous_append_iterator __iter(__out);

    auto __ctxt = format_context::__make(__iter, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __out;
}

inline std::wstring vformat(wstring_view __fmt, wformat_args __args)
{
    std::wstring __out;
    __detail::__contiguous_wappend_iterator __iter(__out);

    auto __ctxt = wformat_context::__make(__iter, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __out;
}

/*inline std::string vformat(const locale& __loc, string_view __fmt, format_args __args)
{
    std::string __out;
    __detail::__contiguous_append_iterator __iter(__out);

    auto __ctxt = format_context::__make(__loc, __iter, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __out;
}

inline std::wstring vformat(const locale& __loc, wstring_view __fmt, wformat_args __args)
{
    std::wstring __out;
    __detail::__contiguous_wappend_iterator __iter(__out);

    auto __ctxt = wformat_context::__make(__loc, __iter, __args);
    __detail::__do_vformat(__fmt, __ctxt);

    return __out;
}*/


} // namespace __XVI_STD_FORMAT_NS

#endif /* ifndef __SYSTEM_CXX_FORMAT_FORMATIMPL_H */

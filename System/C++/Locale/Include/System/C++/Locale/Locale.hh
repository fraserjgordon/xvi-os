#pragma once
#ifndef __SYSTEM_CXX_LOCALE_LOCALE_H
#define __SYSTEM_CXX_LOCALE_LOCALE_H


#include <System/C++/Locale/Private/Config.hh>

#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/SharedPtr.hh>
#include <System/C++/Utility/String.hh>


namespace __XVI_STD_LOCALE_NS
{


// Forward declarations.
struct tm;
class locale;
class ctype_base;
template <class> class ctype;
template <> class ctype<char>;
template <class> class ctype_byname;
class codecvt_base;
template <class, class, class> class codecvt;
template <class, class, class> class codecvt_byname;
template <class _CharT, class _InputIterator = istreambuf_iterator<_CharT>> class num_get;
template <class _CharT, class _OutputIterator = ostreambuf_iterator<_CharT>> class num_put;
template <class> class numpunct;
template <class> class numpunct_byname;
template <class> class collate;
template <class> class collate_byname;
class time_base;
template <class _CharT, class _InputIterator = istreambuf_iterator<_CharT>> class time_get;
template <class _CharT, class _InputIterator = istreambuf_iterator<_CharT>> class time_get_byname;
template <class _CharT, class _OutputIterator = ostreambuf_iterator<_CharT>> class time_put;
template <class _CharT, class _OutputIterator = ostreambuf_iterator<_CharT>> class time_put_byname;
class money_base;
template <class _CharT, class _InputIterator = istreambuf_iterator<_CharT>> class money_get;
template <class _CharT, class _OutputIterator = ostreambuf_iterator<_CharT>> class money_put;
template <class, bool _Intl = false> class moneypunct;
template <class, bool _Intl = false> class moneypunct_byname;
class messages_base;
template <class> class messages;
template <class> class messages_byname;


class locale
{
public:

    class facet;
    class id;

    using category = int;

    static constexpr category none      = 0;
    static constexpr category collate   = 0x0010;
    static constexpr category ctype     = 0x0020;
    static constexpr category monetary  = 0x0040;
    static constexpr category numeric   = 0x0080;
    static constexpr category time      = 0x0100;
    static constexpr category messages  = 0x0200;
    static constexpr category all       = 0x03F0;

    locale() noexcept;
    locale (const locale& __other) noexcept;
    explicit locale(const char* __std_name);
    explicit locale(const string& __std_name);
    locale(const locale& __other, const char* __std_name, category __cat);
    locale(const locale& __other, const string& __std_name, category __cat);
    template <class _Facet> locale(const locale& __other, _Facet* __f);
    locale(const locale& __other, const locale& __one, category __cat);
    
    ~locale();

    const locale& operator=(const locale& __other) noexcept;

    template <class _Facet> locale combine(const locale& __other) const;

    basic_string<char> name() const;

    bool operator==(const locale& __other) const;

    template <class _CharT, class _Traits, class _Allocator>
    bool operator()(const basic_string<_CharT, _Traits, _Allocator>& __s1,
                    const basic_string<_CharT, _Traits, _Allocator>& __s2) const;

    static locale global(const locale&);
    static const locale& classic();


    const facet* __get_facet_by_id(const id&);

private:

    struct __collate_facets
    {
        // Standard.
        __XVI_STD_LOCALE_NS::collate<char>*     __collate_char;
        __XVI_STD_LOCALE_NS::collate<wchar_t>*  __collate_wchar;

        // Conforming extensions.
        __XVI_STD_LOCALE_NS::collate<char8_t>*  __collate_char8;
        __XVI_STD_LOCALE_NS::collate<char16_t>* __collate_char16;
        __XVI_STD_LOCALE_NS::collate<char32_t>* __collate_char32;
    };

    struct __ctype_facets
    {
        // Standard.
        __XVI_STD_LOCALE_NS::ctype<char>*                           __ctype_char;
        __XVI_STD_LOCALE_NS::ctype<wchar_t*>*                       __ctype_wchar;
        __XVI_STD_LOCALE_NS::codecvt<char, char, mbstate_t>*        __codecvt_char_char;
        __XVI_STD_LOCALE_NS::codecvt<char16_t, char8_t, mbstate_t>* __codecvt_char16_char8;
        __XVI_STD_LOCALE_NS::codecvt<char32_t, char8_t, mbstate_t>* __codecvt_char32_char8;
        __XVI_STD_LOCALE_NS::codecvt<wchar_t, char, mbstate_t>*     __codecvt_wchar_char;

        // Conforming extensions.
        __XVI_STD_LOCALE_NS::ctype<char8_t>*                        __ctype_char8;
        __XVI_STD_LOCALE_NS::ctype<char16_t>*                       __ctype_char16;
        __XVI_STD_LOCALE_NS::ctype<char32_t>*                       __ctype_char32;
        __XVI_STD_LOCALE_NS::codecvt<char, char8_t, mbstate_t>*     __ctype_char_char8;
        __XVI_STD_LOCALE_NS::codecvt<wchar_t, char8_t, mbstate_t>*  __ctype_wchar_char8;
    };

    struct __monetary_facets
    {
        __XVI_STD_LOCALE_NS::moneypunct<char>*          __moneypunct_char;
        __XVI_STD_LOCALE_NS::moneypunct<wchar_t>*       __moneypunct_wchar;
        __XVI_STD_LOCALE_NS::moneypunct<char, true>*    __moneypunct_char_intl;
        __XVI_STD_LOCALE_NS::moneypunct<wchar_t, true>* __moneypunct_wchar_intl;
        __XVI_STD_LOCALE_NS::money_get<char>*           __money_get_char;
        __XVI_STD_LOCALE_NS::money_get<wchar_t>*        __money_get_wchar;
        __XVI_STD_LOCALE_NS::money_put<char>*           __money_put_char;
        __XVI_STD_LOCALE_NS::money_put<wchar_t>*        __money_put_wchar;
    };

    struct __numeric_facets
    {
        __XVI_STD_LOCALE_NS::numpunct<char>*        __numpunct_char;
        __XVI_STD_LOCALE_NS::numpunct<wchar_t>*     __numpunct_wchar;
        __XVI_STD_LOCALE_NS::num_get<char>*         __num_get_char;
        __XVI_STD_LOCALE_NS::num_get<wchar_t>*      __num_get_wchar;
        __XVI_STD_LOCALE_NS::num_put<char>*         __num_put_char;
        __XVI_STD_LOCALE_NS::num_put<wchar_t>*      __num_put_wchar;
    };

    struct __time_facets
    {
        __XVI_STD_LOCALE_NS::time_get<char>*        __time_get_char;
        __XVI_STD_LOCALE_NS::time_get<wchar_t>*     __time_get_wchar;
        __XVI_STD_LOCALE_NS::time_put<char>*        __time_put_char;
        __XVI_STD_LOCALE_NS::time_put<wchar_t>*     __time_put_wchar;
    };

    struct __messages_facets
    {
        __XVI_STD_LOCALE_NS::messages<char>*        __messages_char;
        __XVI_STD_LOCALE_NS::messages<wchar_t>*     __messages_wchar;
    };

    struct __std_facets :
        __collate_facets,
        __ctype_facets,
        __monetary_facets,
        __numeric_facets,
        __time_facets,
        __messages_facets
    {
    };
};


class locale::facet
{
protected:

    explicit facet(size_t __refs = 0);
    virtual ~facet();
    facet(const facet&) = delete;
    void operator=(const facet&) = delete;

private:

    __XVI_STD_ATOMIC_NS::atomic<size_t> _M_refcount;
};


class locale::id
{
public:

    id() = default;

    id(const id&) = delete;
    void operator=(const id&) = delete;


    struct __stdlib_id_t {};
    constexpr explicit id(__stdlib_id_t, int __id) :
        _M_id(__id)
    {
    }

    int __get() const noexcept
    {
        return _M_id;
    }

private:

    __XVI_STD_ATOMIC_NS::atomic<int> _M_id = 0;
};


template <class _Facet> const _Facet& use_facet(const locale&);
template <class _Facet> bool has_facet(const locale&) noexcept;


class ctype_base
{
public:

    using mask = uint16_t;

    static constexpr mask space = 0x001;
    static constexpr mask print = 0x002;
    static constexpr mask cntrl = 0x004;
    static constexpr mask upper = 0x008;
    static constexpr mask lower = 0x010;
    static constexpr mask alpha = 0x020;
    static constexpr mask digit = 0x040;
    static constexpr mask punct = 0x080;
    static constexpr mask xdigit = 0x100;
    static constexpr mask blank = 0x200;
    static constexpr mask alnum = alpha | digit;
    static constexpr mask graph = alnum | punct;
};


namespace __detail
{

template <class _CharT> class __ctype :
    public locale::facet,
    public ctype_base
{
public:

    using char_type = _CharT;

    explicit __ctype(size_t __refs = 0);

    bool is(mask __m, _CharT __c) const
    {
        return do_is(__m, __c);
    }

    const _CharT* is(const _CharT* __low, const _CharT* __high, mask* __vec) const
    {
        return do_is(__low, __high, __vec);
    }

    const _CharT* scan_is(mask __m, const _CharT* __low, const _CharT* __high) const
    {
        return do_scan_is(__m, __low, __high);
    }

    const _CharT* scan_not(mask __m, const _CharT* __low, const _CharT* __high) const
    {
        return do_scan_not(__m, __low, __high);
    }

    _CharT toupper(_CharT __c) const
    {
        return do_toupper(__c);
    }

    const _CharT*   toupper(_CharT* __low, const _CharT* __high) const
    {
        return do_toupper(__low, __high);
    }

    _CharT tolower(_CharT __c) const
    {
        return do_tolower(__c);
    }

    const _CharT* tolower(_CharT* __low, const _CharT* __high) const
    {
        return do_tolower(__low, __high);
    }

    _CharT widen(char __c) const
    {
        return do_widen(__c);
    }

    const char* widen(const char* __low, const char* __high, _CharT* __to) const
    {
        return do_widen(__low, __high, __to);
    }

    char narrow(_CharT __c) const
    {
        return do_narrow(__c);
    }

    const _CharT* narrow(const _CharT* __low, const _CharT* __high, char* __to) const
    {
        return do_narrow(__low, __high, __to);
    }

    static locale::id id;

protected:

    virtual bool            do_is(mask __m, _CharT __c) const = 0;
    virtual const _CharT*   do_is(const _CharT* __low, const _CharT* __high, mask* __vec) const = 0;
    virtual const _CharT*   do_scan_is(mask __m, const _CharT* __low, const _CharT* __high) const = 0;
    virtual const _CharT*   do_scan_not(mask __m, const _CharT* __low, const _CharT* __high) const = 0;
    virtual _CharT          do_toupper(_CharT __c) const = 0;
    virtual const _CharT*   do_toupper(_CharT* __low, const _CharT* __high) const = 0;
    virtual _CharT          do_tolower(_CharT __c) const = 0;
    virtual const _CharT*   do_tolower(_CharT* __low, const _CharT* __high) const = 0;
    virtual _CharT          do_widen(char __c) const = 0;
    virtual const char*     do_widen(const char* __low, const char* __high, _CharT* __to) const = 0;
    virtual char            do_narrow(_CharT __c) const = 0;
    virtual const _CharT*   do_narrow(const _CharT* __low, const _CharT* __high, char* __to) const = 0;
};

} // namespace __detail

template <class _CharT>
class ctype :
    public __detail::__ctype<_CharT>
{
public:

    using __detail::__ctype<_CharT>::__ctype;
    
    using mask = ctype_base::mask;

protected:

    virtual bool            do_is(mask __m, _CharT __c) const;
    virtual const _CharT*   do_is(const _CharT* __low, const _CharT* __high, mask* __vec) const;
    virtual const _CharT*   do_scan_is(mask __m, const _CharT* __low, const _CharT* __high) const;
    virtual const _CharT*   do_scan_not(mask __m, const _CharT* __low, const _CharT* __high) const;
    virtual _CharT          do_toupper(_CharT __c) const;
    virtual const _CharT*   do_toupper(_CharT* __low, const _CharT* __high) const;
    virtual _CharT          do_tolower(_CharT __c) const;
    virtual const _CharT*   do_tolower(_CharT* __low, const _CharT* __high) const;
    virtual _CharT          do_widen(char __c) const;
    virtual const char*     do_widen(const char* __low, const char* __high, _CharT* __to) const;
    virtual char            do_narrow(_CharT __c) const;
    virtual const _CharT*   do_narrow(const _CharT* __low, const _CharT* __high, char* __to) const;
};

template <> class ctype<char> :
    public locale::facet,
    public ctype_base
{
public:

    using char_type = char;

    static constexpr size_t table_size = 256;

    explicit ctype(const mask* __table = nullptr, bool __del = false, size_t __refs = 0) :
        facet{__refs},
        ctype_base{},
        _M_table(__table ? __table : classic_table()),
        _M_delete_table(__table && __del)
    {
    }

    bool is(mask __m, char __c) const
    {
        return table()[static_cast<unsigned char>(__c)] & __m;
    }

    const char* is(const char* __low, const char* __high, mask* __vec) const
    {
        while (__low < __high)
            *__vec++ = table()[static_cast<unsigned char>(*__low++)];
        return __high;
    }

    const char* scan_is(mask __m, const char* __low, const char* __high) const
    {
        while (__low < __high)
        {
            if (is(__m, *__low))
                return __low;
            ++__low;
        }

        return nullptr;
    }

    const char* scan_not(mask __m, const char* __low, const char* __high) const
    {
        while (__low < __high)
        {
            if (!is(__m, *__low))
                return __low;
            ++__low;
        }

        return nullptr;
    }

    char toupper(char __c) const
    {
        return do_toupper(__c);
    }

    const char* toupper(char* __low, const char* __high) const
    {
        return do_toupper(__low, __high);
    }

    char tolower(char __c) const
    {
        return do_tolower(__c);
    }

    const char* tolower(char* __low, const char* __high) const
    {
        return do_tolower(__low, __high);
    }

    char widen(char __c) const
    {
        return do_widen(__c);
    }

    const char* widen(const char* __low, const char* __high, char* __to) const
    {
        return do_widen(__low, __high, __to);
    }

    char narrow(char __c, char __default) const
    {
        return do_narrow(__c, __default);
    }

    const char* narrow(const char* __low, const char* __high, char __default, char* __to) const
    {
        return do_narrow(__low, __high, __default, __to);
    }

    const mask* table() const noexcept
    {
        return _M_table;
    }
    

    static const mask* classic_table() noexcept
    {
        return _S_classic_table;
    }

    static locale::id id;

protected:

    ~ctype() override;

    virtual char do_toupper(char __c) const = 0;
    virtual const char* do_toupper(char* __low, const char* __high) const = 0;
    virtual char do_tolower(char __c) const = 0;
    virtual const char* do_tolower(char* __low, const char* __high) const = 0;

    virtual char do_widen(char __c) const = 0;
    virtual const char* do_widen(const char* __low, const char* __high, char* __to) const = 0;
    virtual char do_narrow(char __c, char __default) const = 0;
    virtual const char* do_narrow(const char* __low,const char* __high, char __default, char* __to) const = 0;

private:

    const mask* _M_table;
    bool _M_delete_table;

    static constexpr mask _S_classic_table[table_size] =
    {
        //! @TODO.
    };
};

template <class _CharT> class ctype_byname :
    public ctype<_CharT>
{
public:

    using mask = typename ctype<_CharT>::mask;

    explicit ctype_byname(const char* __locale_name, size_t __refs = 0);

    explicit ctype_byname(const string& __str, size_t __refs = 0) :
        ctype_byname(__str.c_str(), __refs)
    {
    }

protected:

    ~ctype_byname() override = default;
};


// Extern instantiations.
extern template class ctype<char>;
extern template class ctype<wchar_t>;
extern template class ctype_byname<char>;
extern template class ctype_byname<wchar_t>;


class codecvt_base
{
public:

    enum result { ok, partial, error, noconv };
};

template <class _InternT, class _ExternT, class _StateT> class codecvt :
    public locale::facet,
    public codecvt_base
{
public:

    using intern_type   = _InternT;
    using extern_type   = _ExternT;
    using state_type    = _StateT;

    explicit codecvt(size_t __refs = 0);

    result out(state_type& __state,
               const intern_type* __from, const intern_type* __from_end, const intern_type*& __from_next,
               extern_type* __to, extern_type* __to_end, extern_type*& __to_next) const
    {
        return do_out(__state, __from, __from_end, __from_next, __to, __to_end, __to_next);
    }

    result unshift(state_type& __state, extern_type* __to, extern_type* __to_end, extern_type*& __to_next) const
    {
        return do_unshift(__state, __to, __to_end, __to_next);
    }

    result in(state_type& __state,
              const extern_type* __from, const extern_type* __from_end, const extern_type*& __from_next,
              intern_type* __to, intern_type* __to_end, intern_type*& __to_next) const
    {
        return do_in(__state, __from, __from_end, __from_next, __to, __to_end, __to_next);
    }

    int encoding() const noexcept
    {
        return do_encoding();
    }

    bool always_noconv() const noexcept
    {
        return do_always_noconv();
    }

    int length(state_type& __state, const extern_type* __from, const extern_type* __end, size_t __max) const
    {
        return do_length(__state, __from, __end, __max);
    }

    int max_length() const noexcept
    {
        return do_max_length();
    }

    static locale::id id;

protected:

    ~codecvt() override;

    virtual result do_out(state_type& __state,
                          const intern_type* __from, const intern_type* __from_end, const intern_type*& __from_next,
                          extern_type* __to, extern_type* __to_end, extern_type*& __to_next) const;

    virtual result do_unshift(state_type& __state, extern_type* __to, extern_type* __to_end, extern_type*& __to_next) const;

    virtual result do_in(state_type& __state,
                         const extern_type* __from, const extern_type* __from_end, const extern_type*& __from_next,
                         intern_type* __to, intern_type* __to_end, intern_type*& __to_next) const;

    virtual int do_encoding() const noexcept;
    virtual bool do_always_noconv() const noexcept;
    virtual int do_length(state_type& __state, const extern_type* __from, const extern_type* __end, size_t __max) const;
    virtual int do_max_length() const noexcept;
};

template <class _InternT, class _ExternT, class _StateT> class codecvt_byname :
    public codecvt<_InternT, _ExternT, _StateT>
{
public:

    explicit codecvt_byname(const char* __name, size_t __refs = 0);

    explicit codecvt_byname(const string& __name, size_t __refs = 0) :
        codecvt_byname{__name.c_str(), __refs}
    {
    }

protected:

    ~codecvt_byname() override = default;
};


// Extern instantiations.
extern template class codecvt<char, char, mbstate_t>;
extern template class codecvt<char16_t, char8_t, mbstate_t>;
extern template class codecvt<char32_t, char8_t, mbstate_t>;
extern template class codecvt<wchar_t, char, mbstate_t>;
extern template class codecvt_byname<char, char, mbstate_t>;
extern template class codecvt_byname<char16_t, char8_t, mbstate_t>;
extern template class codecvt_byname<char32_t, char8_t, mbstate_t>;
extern template class codecvt_byname<wchar_t, char, mbstate_t>;


template <class _CharT, class _InputIterator>
class num_get;

template <class _CharT, class _OutputIterator>
class num_put;

template <class _CharT> class numpunct;

template <class _CharT> class numpunct_byname;

template <class _CharT> class collate;

template <class _CharT> class collate_byname;

class time_base;

template <class _CharT, class _InputIterator>
class time_get;

template <class _CharT, class _InputIterator>
class time_get_byname;

template <class _CharT, class _OutputIterator>
class time_put;

template <class _CharT, class _OutputIterator>
class time_put_byname;

class money_base;

template <class _CharT, class _InputIterator>
class money_get;

template <class _CharT, class _OutputIterator>
class money_put;

template <class _CharT, bool _Intl>
class moneypunct;

template <class _CharT, bool _Intl>
class moneypunct_byname;

class messages_base;

template <class _CharT> class messages;

template <class _CharT> class messages_byname;


template <class _CharT> bool isspace(_CharT __c, const locale& __loc);
template <class _CharT> bool isprint(_CharT __c, const locale& __loc);
template <class _CharT> bool iscntrl(_CharT __c, const locale& __loc);
template <class _CharT> bool isupper(_CharT __c, const locale& __loc);
template <class _CharT> bool islower(_CharT __c, const locale& __loc);
template <class _CharT> bool isalpha(_CharT __c, const locale& __loc);
template <class _CharT> bool isdigit(_CharT __c, const locale& __loc);
template <class _CharT> bool ispunct(_CharT __c, const locale& __loc);
template <class _CharT> bool isxdigit(_CharT __c, const locale& __loc);
template <class _CharT> bool isalnum(_CharT __c, const locale& __loc);
template <class _CharT> bool isgraph(_CharT __c, const locale& __loc);
template <class _CharT> bool isblank(_CharT __c, const locale& __loc);
template <class _CharT> _CharT toupper(_CharT __c, const locale& __loc);
template <class _CharT> _CharT tolower(_CharT __c, const locale& __loc);



} // namespace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_LOCALE_H */

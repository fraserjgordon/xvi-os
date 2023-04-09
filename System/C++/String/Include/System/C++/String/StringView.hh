#ifndef __SYSTEM_CXX_STRING_STRINGVIEW_H
#define __SYSTEM_CXX_STRING_STRINGVIEW_H


#include <System/C++/Core/IteratorAdaptors.hh>
#include <System/C++/Core/IteratorUtils.hh>
#include <System/C++/Core/MemoryAlgorithms.hh>
#include <System/C++/Core/StdExcept.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/String/Private/Config.hh>
#include <System/C++/String/CharTraits.hh>


namespace __XVI_STD_STRING_NS_DECL
{


// Forward declarations (mutual dependency between out_of_range and basic_string_view).
class out_of_range;


template <class _CharT, class _Traits = char_traits<_CharT>>
class basic_string_view
{
public:

    using traits_type               = _Traits;
    using value_type                = _CharT;
    using pointer                   = value_type*;
    using const_pointer             = const value_type*;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using const_iterator            = const_pointer;
    using iterator                  = const_iterator;
    using const_reverse_iterator    = __XVI_STD_NS::reverse_iterator<const_iterator>;
    using reverse_iterator          = const_reverse_iterator;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;

    static constexpr size_type npos = size_type(-1);

    constexpr basic_string_view() noexcept = default;
    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(const _CharT* __str)
        : _M_ptr(__str),
          _M_len(_Traits::length(__str))
    {
    }

    basic_string_view(nullptr_t) = delete;

    constexpr basic_string_view(const _CharT* __str, size_t __len)
        : _M_ptr(__str), _M_len(__len)
    {
    }

    template <contiguous_iterator _It, sized_sentinel_for<_It> _End>
        requires is_same_v<iter_value_t<_It>, _CharT>
            && (!is_convertible_v<_End, size_type>)
    constexpr basic_string_view(_It __begin, _End __end)
        : _M_ptr(to_address(__begin)), _M_len(__end - __begin)
    {
    }

    template <class _R>
        requires (!is_same_v<remove_cvref_t<_R>, basic_string_view>)
            && ranges::contiguous_range<_R>
            && ranges::sized_range<_R>
            && is_same_v<ranges::range_value_t<_R>, _CharT>
            && (!is_convertible_v<_R, const _CharT*>)
            && (!(requires (remove_cvref_t<_R> __d) { __d.operator basic_string_view<_CharT, _Traits>(); }))
            && (!(requires { typename remove_reference_t<_R>::traits_type; })
                || is_same_v<_Traits, typename remove_reference_t<_R>::traits_type>)
    constexpr basic_string_view(_R&& __r) :
        _M_ptr(ranges::data(__r)), _M_len(ranges::size(__r))
    {
    }

    constexpr const_iterator begin() const noexcept
    {
        return _M_ptr;
    }

    constexpr const_iterator end() const noexcept
    {
        return _M_ptr + _M_len;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    friend constexpr const_iterator begin(basic_string_view __sv) noexcept
    {
        return __sv.begin();
    }

    friend constexpr const_iterator end(basic_string_view __sv) noexcept
    {
        return __sv.end();
    }

    constexpr size_type size() const noexcept
    {
        return _M_len;
    }

    constexpr size_type length() const noexcept
    {
        return _M_len;
    }

    constexpr size_type max_size() const noexcept
    {
        return numeric_limits<size_t>::max();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return _M_len == 0;
    }

    constexpr const_reference operator[](size_type __pos) const
    {
        return _M_ptr[__pos];
    }

    constexpr const_reference at(size_type __pos) const
    {
        if (__pos >= size())
            __out_of_range();

        return _M_ptr[__pos];
    }

    constexpr const_reference front() const
    {
        return _M_ptr[0];
    }

    constexpr const_reference back() const
    {
        return _M_ptr[_M_len - 1];
    }

    constexpr const_pointer data() const noexcept
    {
        return _M_ptr;
    }

    constexpr void remove_prefix(size_type __n)
    {
        _M_ptr += __n;
        _M_len -= __n;
    }

    constexpr void remove_suffix(size_type __n)
    {
        _M_len -= __n;
    }

    constexpr void swap(basic_string_view& __sv) noexcept
    {
        using __XVI_STD_NS::swap;
        swap(_M_ptr, __sv._M_ptr);
        swap(_M_len, __sv._M_len);
    }

    constexpr size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const
    {
        if (__pos > size())
            __out_of_range();
        
        auto __rlen = min(__n, size() - __pos);
        _Traits::copy(__s, data() + __pos, __rlen);
        return __rlen;
    }

    constexpr basic_string_view substr(size_type __pos = 0, size_type __n = npos) const
    {
        if (__pos > size())
            __out_of_range();

        auto __rlen = min(__n, size() - __pos);
        return basic_string_view(data() + __pos, __rlen);
    }

    constexpr int compare(basic_string_view __s) const noexcept
    {
        auto __rlen = min(size(), __s.size());
        int __res = _Traits::compare(data(), __s.data(), __rlen);

        if (__res != 0)
            return __res;
        
        if (size() < __s.size())
            return numeric_limits<int>::min();
        else if (size() > __s.size())
            return numeric_limits<int>::max();
        else
            return 0;
    }

    constexpr int compare(size_type __pos1, size_type __n1, basic_string_view __s) const
    {
        return substr(__pos1, __n1).compare(__s);
    }

    constexpr int compare(size_type __pos1, size_type __n1, basic_string_view __s, size_type __pos2, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(__s.substr(__pos2, __n2));
    }

    constexpr int compare(const _CharT* __s) const
    {
        return compare(basic_string_view(__s));
    }

    constexpr int compare(size_type __pos1, size_type __n1, const _CharT* __s) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s));
    }

    constexpr int compare(size_type __pos1, size_type __n1, const _CharT* __s, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s, __n2));
    }

    constexpr bool starts_with(basic_string_view __x) const noexcept
    {
        return compare(0, __x.size(), __x) == 0;
    }

    constexpr bool starts_with(_CharT __x) const noexcept
    {
        return starts_with(basic_string_view(addressof(__x), 1));
    }

    constexpr bool starts_with(const _CharT* __s) const
    {
        return starts_with(basic_string_view(__s));
    }

    constexpr bool ends_with(basic_string_view __x) const noexcept
    {
        return size() >= __x.size() && compare(size() - __x.size(), npos, __x) == 0;
    }

    constexpr bool ends_with(_CharT __x) const noexcept
    {
        return ends_with(basic_string_view(addressof(__x), 1));
    }

    constexpr bool ends_with(const _CharT* __s) const
    {
        return ends_with(basic_string_view(__s));
    }

    constexpr size_type find(basic_string_view __s, size_type __pos = 0) const noexcept
    {
        if (__pos >= size())
            return npos;
        
        if (__s.size() == 1)
        {
            auto __p = _Traits::find(data() + __pos, size() - __pos, __s.front());
            if (__p == nullptr)
                return npos;
            return static_cast<size_type>(__p - begin());
        }

        // Boyer-Moore-Horspool substring search.
        __search_table_t __tbl = {};
        __generate_search_table(__tbl, __s);

        size_t __offset = __pos;
        size_t __length = size();
        while (__length - __offset >= __s.size())
        {
            size_t __i = __s.size() - 1;
            while (_M_ptr[__offset + __i] == __s[__i])
            {
                if (__i == 0)
                    return __offset;
                --__i;
            }

            __offset += __tbl[__hash(_M_ptr[__offset + __s.size() - 1])];
        }

        return npos;
    }

    constexpr size_type find(_CharT __c, size_type __pos = 0) const noexcept
    {
        return find(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type find(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return find(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type find(const _CharT* __s, size_type __pos = 0) const
    {
        return find(basic_string_view(__s), __pos);
    }

    //! @TODO: implement.
    constexpr size_type rfind(basic_string_view __s, size_type __pos = npos) const noexcept;

    constexpr size_type rfind(_CharT __c, size_type __pos = npos) const noexcept
    {
        return rfind(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return rfind(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type rfind(const _CharT* __s, size_type __pos = npos) const
    {
        return rfind(basic_string_view(__s), __pos);
    }

    constexpr size_type find_first_of(basic_string_view __s, size_type __pos = 0) const noexcept
    {
        if (__pos >= size())
            return npos;
        
        if (__s.size() == 1)
        {
            auto __p = _Traits::find(data() + __pos, size() - __pos, __s.front());
            if (__p == nullptr)
                return npos;
            return static_cast<size_type>(__p - data());
        }

        __hash_table_t __tbl = {};
        __generate_hash_table(__tbl, __s);
        for (const_pointer __p = begin() + __pos; __p != end(); ++__p)
        {
            if (__in_table(__tbl, *__p))
            {
                if constexpr (__hash_is_exact())
                    return static_cast<size_type>(__p - begin());

                if (_Traits::find(__s.data(), __s.size(), *__p) != nullptr)
                    return static_cast<size_type>(__p - begin());
            }
        }

        return npos;
    }

    constexpr size_type find_first_of(_CharT __c, size_type __pos = 0) const noexcept
    {
        return find_first_of(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return find_first_of(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type find_first_of(const _CharT* __s, size_type __pos = 0) const
    {
        return find_first_of(basic_string_view(__s), __pos);
    }

    constexpr size_type find_last_of(basic_string_view __s, size_type __pos = npos) const noexcept
    {
        difference_type __offset = 0;
        if (__pos < size())
            __offset = static_cast<difference_type>(size() - __pos);
        
        if (__s.size() == 1)
        {
            _CharT __c = __s.front();
            for (auto __p = rbegin() + __offset; __p != rend(); ++__p)
            {
                if (*__p == __c)
                    return static_cast<size_type>(addressof(*__p) - begin());
            }

            return npos;
        }
        
        __hash_table_t __tbl = {};
        __generate_hash_table(__tbl, __s);
        for (auto __p = rbegin() + __offset; __p != rend(); ++__p)
        {
            if (__in_table(__tbl, *__p))
            {
                if constexpr (__hash_is_exact())
                    return static_cast<size_type>(addressof(*__p) - begin());

                if (_Traits::find(__s.data(), __s.size(), *__p) != nullptr)
                    return static_cast<size_type>(addressof(*__p) - begin());
            }
        }

        return npos;
    }

    constexpr size_type find_last_of(_CharT __c, size_type __pos = npos) const noexcept
    {
        return find_last_of(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return find_last_of(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type find_last_of(const _CharT* __s, size_type __pos = npos) const
    {
        return find_last_of(basic_string_view(__s), __pos);
    }

    constexpr size_type find_first_not_of(basic_string_view __s, size_type __pos = 0) const noexcept
    {
        if (__pos >= size())
            return npos;
        
        if (__s.size() == 1)
        {
            auto __p = _Traits::find(data() + __pos, size() - __pos, __s.front());
            if (__p == nullptr)
                return npos;
            return __p - data();
        }

        __hash_table_t __tbl = {};
        __generate_hash_table(__tbl, __s);
        for (const_pointer __p = begin() + __pos; __p != end(); ++__p)
        {
            if (!__in_table(__tbl, *__p))
            {
                if constexpr (__hash_is_exact())
                    return __p - begin();

                if (_Traits::find(__s.data(), __s.size(), *__p) == nullptr)
                    return __p - begin();
            }
        }

        return npos;
    }

    constexpr size_type find_first_not_of(_CharT __c, size_type __pos = 0) const noexcept
    {
        return find_first_not_of(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return find_first_not_of(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type find_first_not_of(const _CharT* __s, size_type __pos = 0) const
    {
        return find_first_not_of(basic_string_view(__s), __pos);
    }

    constexpr size_type find_last_not_of(basic_string_view __s, size_type __pos = npos) const noexcept
    {
        size_type __offset = 0;
        if (__pos < size())
            __offset = size() - __pos;
        
        if (__s.size() == 1)
        {
            _CharT __c = __s.front();
            for (auto __p = rbegin() + __offset; __p != rend(); ++__p)
            {
                if (*__p != __c)
                    return addressof(*__p) - begin();
            }

            return npos;
        }
        
        __hash_table_t __tbl = {};
        __generate_hash_table(__tbl, __s);
        for (auto __p = rbegin() + __offset; __p != rend(); ++__p)
        {
            if (!__in_table(__tbl, *__p))
            {
                if constexpr (__hash_is_exact())
                    return addressof(*__p) - begin();

                if (_Traits::find(__s.data(), __s.size(), *__p) == nullptr)
                    return addressof(*__p) - begin();
            }
        }

        return npos;
    }

    constexpr size_type find_last_not_of(_CharT __c, size_type __pos = npos) const noexcept
    {
        return find_last_not_of(basic_string_view(addressof(__c), 1), __pos);
    }

    constexpr size_type find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return find_last_not_of(basic_string_view(__s, __n), __pos);
    }

    constexpr size_type find_last_not_of(const _CharT* __s, size_type __pos = npos) const
    {
        return find_last_not_of(basic_string_view(__s), __pos);
    }

    constexpr bool contains(basic_string_view __s) const noexcept
    {
        return find(__s) != npos;
    }

    constexpr bool contains(_CharT __x) const noexcept
    {
        return find(__x) != npos;
    }

    constexpr bool contains(const _CharT* __s) const
    {
        return find(__s) != npos;
    }


private:

    const _CharT* _M_ptr = nullptr;
    size_t        _M_len = 0;

    using __hash_table_t = bool[256];

    static constexpr uint8_t __hash(_CharT __c)
    {
        if constexpr (sizeof(_CharT) <= sizeof(uint8_t))
            return static_cast<uint8_t>(__c);
        
        //! @TODO: better hashing algorithm for UTF-16 and UTF-32.
        return static_cast<uint8_t>(__c & 0xFF);
    }

    static constexpr bool __hash_is_exact()
    {
        return sizeof(_CharT) <= sizeof(uint8_t);
    }

    static constexpr void __generate_hash_table(__hash_table_t& __tab, basic_string_view __needle)
    {
        __XVI_STD_NS::uninitialized_fill_n(__tab, 256, false);

        for (_CharT __c : __needle)
            __tab[__hash(__c)] = true;
    }

    static constexpr bool __in_table(const __hash_table_t& __tab, _CharT __c)
    {
        return __tab[__hash(__c)];
    }

    using __search_table_t = size_type[256];

    static constexpr void __generate_search_table(__search_table_t& __tab, basic_string_view __needle)
    {
        for (size_t __i = 0; __i < 256; ++__i)
            __tab[__i] = __needle.length();

        for (size_t __i = 0; __i < __needle.length() - 1; ++__i)
            __tab[__hash(__needle[__i])] = __needle.length() - 1 - __i;
    }

    [[noreturn]] static void __out_of_range()
    {
        throw out_of_range("invalid index into string_view");
    }
};

template <contiguous_iterator _It, sized_sentinel_for<_It> _End>
basic_string_view(_It, _End) -> basic_string_view<iter_value_t<_It>>;

template <ranges::contiguous_range _R>
basic_string_view(_R&&) -> basic_string_view<ranges::range_value_t<_R>>;


template <class _CharT, class _Traits>
inline constexpr bool ranges::enable_view<basic_string_view<_CharT, _Traits>> = true;

template <class _CharT, class _Traits>
inline constexpr bool ranges::enable_borrowed_range<basic_string_view<_CharT, _Traits>> = true;


namespace __detail
{

template <class>
struct __char_comparison_category { using __type = weak_ordering; };

template <class _Traits>
    requires requires { typename _Traits::comparison_category; }
struct __char_comparison_category<_Traits> { using __type = typename _Traits::comparison_category; };

template <class _Traits>
using __char_comparison_category_t = typename __char_comparison_category<_Traits>::__type;

} // namespace __detail


template <class _CharT, class _Traits>
constexpr bool operator==(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) == 0;
}

template <class _CharT, class _Traits>
constexpr bool operator==(basic_string_view<_CharT, _Traits> __lhs, type_identity_t<basic_string_view<_CharT, _Traits>> __rhs) noexcept
{
    return __lhs.compare(__rhs) == 0;
}

template <class _CharT, class _Traits>
constexpr auto operator<=>(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    using _R = __detail::__char_comparison_category_t<_Traits>;
    return static_cast<_R>(__lhs.compare(__rhs) <=> 0);
}

template <class _CharT, class _Traits>
constexpr auto operator<=>(basic_string_view<_CharT, _Traits> __lhs, type_identity_t<basic_string_view<_CharT, _Traits>> __rhs) noexcept
{
    using _R = __detail::__char_comparison_category_t<_Traits>;
    return static_cast<_R>(__lhs.compare(__rhs) <=> 0);
}


using string_view    = basic_string_view<char>;
using u8string_view  = basic_string_view<char8_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using wstring_view   = basic_string_view<wchar_t>;


inline namespace literals
{
inline namespace string_view_literals
{

// GCC doesn't always detect this as a system header properly and throws warnings about these suffixes.
#pragma GCC diagnostic push
#ifdef __llvm__
#  pragma GCC diagnostic ignored "-Wuser-defined-literals"
#else
#  pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif

constexpr string_view operator""sv(const char* __str, size_t __len) noexcept
{
    return string_view{__str, __len};
}

constexpr u8string_view operator""sv(const char8_t* __str, size_t __len) noexcept
{
    return u8string_view{__str, __len};
}

constexpr u16string_view operator""sv(const char16_t* __str, size_t __len) noexcept
{
    return u16string_view{__str, __len};
}

constexpr u32string_view operator""sv(const char32_t* __str, size_t __len) noexcept
{
    return u32string_view{__str, __len};
}

constexpr wstring_view operator""sv(const wchar_t* __str, size_t __len) noexcept
{
    return wstring_view{__str, __len};
}

#pragma GCC diagnostic pop

} // namespace string_view_literals
} // namespace literals


} // namespace __XVI_STD_STRING_NS_DECL


#endif /* ifndef __SYSTEM_CXX_STRING_STRINGVIEW_H */

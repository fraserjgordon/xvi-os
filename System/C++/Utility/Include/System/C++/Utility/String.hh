#pragma once
#ifndef __SYSTEM_CXX_UTILITY_STRING_H
#define __SYSTEM_CXX_UTILITY_STRING_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>

#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/CharTraits.hh>
#include <System/C++/Utility/ContainerUtils.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/RangesUtils.hh>
#include <System/C++/Utility/StringFwd.hh>
#include <System/C++/Utility/StringView.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{


template <class _CharT, class _SizeType>
class __basic_string_storage
{
protected:
    
    // Number of characters storable inline.
    //
    // The '- 2' is for the terminator and the size counter.
    static constexpr _SizeType _MaxInline = 4*sizeof(void*) / sizeof(_CharT) - 2;

    // Flags byte is zero if the string is inline (small string optimisation). The low bit is set for out-of-line
    // strings.
    //
    // Note that for inline strings, the flags byte doubles as the NUL terminator when the length of the inline string
    // is equal to the max inline length.

    template <size_t _Padding>
    struct __padded_flags_t
    {
        _CharT                          __padding[_Padding] = {};
        std::make_unsigned_t<_CharT>    __value = 0;
    };

    struct __unpadded_flags_t
    {
        std::make_unsigned_t<_CharT>    __value = 0;
    };

    template <size_t _Padding = (sizeof(void*) - sizeof(_CharT)) / sizeof(_CharT)>
    using __flags_t = std::conditional_t<_Padding == 0, __unpadded_flags_t, __padded_flags_t<_Padding>>;

    constexpr __basic_string_storage() {};
    constexpr ~__basic_string_storage() {};

    union
    {
        struct
        {
            _SizeType           _M_length;
            _SizeType           _M_capacity;
            _CharT*             _M_memory;
            __flags_t<>         _M_padded_flags;
        };

        // Small string optimisation.
        struct
        {
            make_unsigned_t<_CharT> _M_inline_length = 0;
            _CharT                  _M_inline[_MaxInline] = {};
            __unpadded_flags_t      _M_flags = {};
        };
    };
};


} // namespace __detail


// Forward declarations.
template <class, class> class basic_string_view;


template <class _CharT, class _Traits, class _Allocator>
class basic_string :
    private __detail::__basic_string_storage<_CharT, typename allocator_traits<_Allocator>::size_type>
{
public:

    using traits_type       = _Traits;
    using value_type        = _CharT;
    using allocator_type    = _Allocator;
    using size_type         = typename allocator_traits<_Allocator>::size_type;
    using difference_type   = typename allocator_traits<_Allocator>::difference_type;
    using pointer           = typename allocator_traits<_Allocator>::pointer;
    using const_pointer     = typename allocator_traits<_Allocator>::const_pointer;
    using reference         = value_type&;
    using const_reference   = const value_type&;

    using iterator                  = pointer;
    using const_iterator            = const_pointer;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

    using _Storage = __detail::__basic_string_storage<_CharT, size_type>;

    static constexpr size_type npos = static_cast<size_type>(-1);

    constexpr basic_string() noexcept(noexcept(_Allocator()))
        : basic_string(_Allocator())
    {
    }

    constexpr explicit basic_string(const _Allocator& __a) noexcept
        : _Storage{},
          _M_allocator(__a)
    {
        __constexpr_setup();
    }

    constexpr basic_string(const basic_string& __s)
        : basic_string(__s.get_allocator())
    {
        append(__s);
    }

    constexpr basic_string(basic_string&& __s) noexcept
        : _M_allocator(std::move(__s._M_allocator))
    {
        __constexpr_setup();

        if (__s.__is_inline())
        {
            append(__s);
        }
        else
        {
            __mark_not_inline();
            _M_length = __s._M_length;
            _M_capacity = __s._M_capacity;
            _M_memory = __s._M_memory;
            __s.__mark_inline();
        }
    }

    constexpr basic_string(const basic_string& __s, size_type __pos, const _Allocator& __a = _Allocator())
        : basic_string(__s, __pos, npos, __a)
    {
    }

    constexpr basic_string(const basic_string& __s, size_type __pos, size_type __n, const _Allocator& __a = _Allocator())
        : basic_string(basic_string_view<_CharT, _Traits>(__s).substr(__pos, __n), __a)
    {
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
    constexpr basic_string(const _T& __t, size_type __pos, size_type __n, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        operator=(basic_string(__sv.substr(__pos, __n), __a));
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string(const _T& __t, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        operator=(basic_string(__sv.data(), __sv.size(), __a));
    }

    constexpr basic_string(const _CharT* __s, size_type __n, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        append(__s, __n);
    }

    basic_string(std::nullptr_t) = delete;

    constexpr basic_string(const _CharT* __s, const _Allocator& __a = _Allocator())
        requires __detail::__maybe_allocator<_Allocator>
        : basic_string(__s, _Traits::length(__s), __a)
    {
    }

    basic_string(size_type __n, _CharT __c, const _Allocator& __a = _Allocator())
        requires __detail::__maybe_allocator<_Allocator>
        : basic_string(__a)
    {
        reserve(__n);
        append(__n, __c);
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr basic_string(_InputIterator __begin, _InputIterator __end, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        assign(__begin, __end);
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string(from_range_t, _R&& __rg, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        assign(from_range, std::forward<_R>(__rg));
    }

    constexpr basic_string(initializer_list<_CharT> __il, const _Allocator& __a = _Allocator())
        : basic_string(__il.begin(), __il.end(), __a)
    {
    }

    constexpr basic_string(const basic_string& __s, const _Allocator& __a)
        : basic_string(__a)
    {
        append(__s);
    }

    constexpr basic_string(basic_string&& __s, const _Allocator& __a)
        : basic_string(__a)
    {
        if (__s.__is_inline())
        {
            append(__s);
        }
        else
        {
            __mark_not_inline();
            _M_length = __s._M_length;
            _M_capacity = __s._M_capacity;
            _M_memory = __s._M_memory;
            __s.__mark_inline();
        }
    }

    constexpr ~basic_string()
    {
        __deallocate();
    }

    constexpr basic_string& operator=(const basic_string& __s)
    {
        if (addressof(__s) == this)
            return *this;
        
        clear();
        return append(__s);
    }

    constexpr basic_string& operator=(basic_string&& __s)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                 || allocator_traits<_Allocator>::is_always_equal::value)
    {
        if (addressof(__s) == this)
            return *this;

        __deallocate();

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value)
            _M_allocator = std::move(__s._M_allocator);

        if (__s.__is_inline())
        {
            // Copy rather than a move in this case.
            append(__s);
            return *this;
        }

        __mark_not_inline();
        _M_length = __s._M_length;
        _M_capacity = __s._M_capacity;
        _M_memory = __s._M_memory;
        __s.__mark_inline();

        return *this;
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& operator=(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv);
    }

    constexpr basic_string& operator=(const _CharT* __s)
    {
        return *this = basic_string_view<_CharT, _Traits>(__s);
    }

    basic_string& operator=(std::nullptr_t) = delete;

    constexpr basic_string& operator=(_CharT __c)
    {
        return *this = basic_string_view<_CharT, _Traits>(addressof(__c), 1);
    }

    constexpr basic_string& operator=(initializer_list<_CharT> __il)
    {
        return *this = basic_string_view<_CharT, _Traits>(__il.begin(), __il.size());
    }

    constexpr iterator begin() noexcept
    {
        return data();
    }

    constexpr const_iterator begin() const noexcept
    {
        return data();
    }

    constexpr iterator end() noexcept
    {
        return data() + size();
    }

    constexpr const_iterator end() const noexcept
    {
        return data() + size();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return revese_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    constexpr size_type size() const noexcept
    {
        return __is_inline() ? static_cast<size_type>(_M_inline_length) : _M_length;
    }

    constexpr size_type length() const noexcept
    {
        return size();
    }

    constexpr size_type max_size() const noexcept
    {
        // Divided by an additional constant factor to prevent overflow when resizing.
        return numeric_limits<size_type>::max() / (sizeof(_CharT) * 4);
    }

    constexpr void resize(size_type __n, _CharT __c)
    {
        if (__n < size())
            __truncate_in_place(__n);
        else if (__n > size())
            append(__n - size(), __c);
    }

    constexpr void resize(size_type __n)
    {
        resize(__n, _CharT());
    }

    template <class _Operation>
    constexpr void resize_and_overwrite(size_type __n, _Operation __op)
    {
        auto __o = size();
        auto __k = min<size_type>(__o, __n);
        auto __g = (__n > __o) ? __n - __o : 0;

        __reallocate(__n, __k, __g);

        auto __r = std::move(__op)(data(), __n);

        __truncate_in_place(__r);
    }

    constexpr size_type capacity() const noexcept
    {
        return __is_inline() ? _MaxInline : _M_capacity;
    }

    constexpr void reserve(size_type __n)
    {
        if (__n > max_size())
            __XVI_CXX_UTILITY_THROW(length_error("std::basic_string::reserve(n) with n > max_size()"));

        if (capacity() < __n)
            (__n);
    }

    constexpr void shrink_to_fit()
    {
        __reallocate(size());
    }

    constexpr void clear() noexcept
    {
        if (__is_inline())
        {
            _M_inline_length = 0;
            _M_inline[0] = _CharT(0);
        }
        else
        {
            _M_length = 0;
            _M_memory[0] = _CharT(0);
        }
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr const_reference operator[](size_type __pos) const
    {
        return data()[__pos];
    }
    
    constexpr reference operator[](size_type __pos)
    {
        return data()[__pos];
    }

    constexpr const_reference at(size_type __pos) const
    {
        if (__pos >= size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid index for std::basic_string::at"));

        return operator[](__pos);
    }

    constexpr reference at(size_type __pos)
    {
        if (__pos >= size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid index for std::basic_string::at"));

        return operator[](__pos);
    }

    constexpr const _CharT& front() const
    {
        return data()[0];
    }

    constexpr _CharT& front()
    {
        return data()[0];
    }

    constexpr const _CharT& back() const
    {
        return data()[size() - 1];
    }

    constexpr _CharT& back()
    {
        return data()[size() - 1];
    }

    constexpr basic_string& operator+=(const basic_string& __s)
    {
        return append(__s);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& operator+=(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return append(__sv);
    }

    constexpr basic_string& operator+=(const _CharT* __s)
    {
        return append(__s);
    }

    constexpr basic_string& operator+=(_CharT __c)
    {
        return append(size_type{1}, __c);
    }

    constexpr basic_string& operator+=(initializer_list<_CharT> __il)
    {
        return append(__il);
    }

    constexpr basic_string& append(const basic_string& __str)
    {
        return append(__str.data(), __str.size());
    }

    constexpr basic_string& append(const basic_string& __str, size_type __pos, size_type __n = npos)
    {
        return append(basic_string_view<_CharT, _Traits>(__str).substr(__pos, __n));
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& append(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return append(__sv.data(), __sv.size());
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& append(const _T& __t, size_type __pos, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return append(__sv.substr(__pos, __n));
    }

    constexpr basic_string& append(const _CharT* __s, size_type __n)
    {
        __ensure_space(size() + __n);
        _Traits::copy(end(), __s, __n);
        
        if (__is_inline())
        {
            _M_inline_length += __n;
            if (_M_inline_length < _MaxInline)
                _M_inline[_M_inline_length] = _CharT(0);
        }
        else
        {
            _M_length += __n;
            _M_memory[_M_length] = _CharT(0);
        }

        return *this;
    }

    constexpr basic_string& append(const _CharT* __s)
    {
        return append(__s, _Traits::length(__s));
    }

    constexpr basic_string& append(size_type __n, _CharT __c)
    {
        __ensure_space(size() + __n);
        _Traits::assign(end(), __n, __c);

        if (__is_inline())
        {
            _M_inline_length += __n;
            if (_M_inline_length < _MaxInline)
                _M_inline[_M_inline_length] = _CharT(0);
        }
        else
        {
            _M_length += __n;
            _M_memory[_M_length] = _CharT(0);
        }

        return *this;
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr basic_string& append(_InputIterator __first, _InputIterator __last)
    {
        if constexpr (std::is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        {
            auto __dist = distance(__first, __last);
            reserve(size() + __dist);
        }

        for (; __first != __last; ++__first)
            append(static_cast<_CharT>(*__first));
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string& append_range(_R&& __r)
    {
        if constexpr (ranges::sized_range<_R>)
        {
            auto __len = ranges::distance(__r);
            reserve(size() + __len);
        }

        for (auto __first = ranges::begin(__r), __last = ranges::end(__r); __first != __last; ++__first)
            append(static_cast<_CharT>(*__first));
    }

    constexpr basic_string& append(initializer_list<_CharT> __il)
    {
        return append(__il.begin(), __il.size());
    }

    constexpr void push_back(_CharT __c)
    {
        append(size_type{1}, __c);
    }

    constexpr basic_string& assign(const basic_string& __s)
    {
        return *this = __s;
    }

    constexpr basic_string& assign(basic_string&& __s)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                 || allocator_traits<_Allocator>::is_always_equal::value)
    {
        return *this = std::move(__s);
    }

    constexpr basic_string& assign(const basic_string& __s, size_type __pos, size_type __n = npos)
    {
        return assign(basic_string_view<_CharT, _Traits>(__s).substr(__pos, __n));
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& assign(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv.data(), __sv.size());
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& assign(const _T& __t, size_type __pos, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv.substr(__pos, __n));
    }

    constexpr basic_string& assign(const _CharT* __s, size_type __n)
    {
        __ensure_space(__n);
        clear();
        return append(__s, __n);
    }

    constexpr basic_string& assign(const _CharT* __s)
    {
        return assign(__s, _Traits::length(__s));
    }

    constexpr basic_string& assign(size_type __n, _CharT __c)
    {
        __ensure_space(__n);
        clear();
        return append(__n, __c);
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr basic_string& assign(_InputIterator __first, _InputIterator __last)
    {
        clear();
        append(__first, __last);
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string& assign_range(_R&& __r)
    {
        clear();
        append_range(std::forward<_R>(__r));
    }

    constexpr basic_string& assign(initializer_list<_CharT> __il)
    {
        return assign(__il.begin(), __il.size());
    }

    constexpr basic_string& insert(size_type __pos, const basic_string& __str)
    {
        return insert(__pos, __str.data(), __str.size());
    }

    constexpr basic_string& insert(size_type __pos, const basic_string& __str, size_type __pos2, size_type __n = npos)
    {
        return insert(__pos, basic_string_view<_CharT, _Traits>(__str), __pos2, __n);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& insert(size_type __pos, const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return insert(__pos, __sv.data(), __sv.size());
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& insert(size_type __pos, const _T& __t, size_type __pos2, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return insert(__pos, __sv.substr(__pos2, __n));
    }

    constexpr basic_string& insert(size_type __pos, const _CharT* __s, size_type __n)
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::insert"));

        if (__n > max_size() - size())
            __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::insert"));

        __ensure_space_and_create_gap(size() + __n, __pos, __n);

        _Traits::copy(data() + __pos, __s, __n);

        return *this;
    }

    constexpr basic_string& insert(size_type __pos, const _CharT* __s)
    {
        return insert(__pos, __s, _Traits::length(__s));
    }

    constexpr basic_string& insert(size_type __pos, size_type __n, _CharT __c)
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::insert"));

        if (__n > max_size() - size())
            __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::insert"));

        __ensure_space_and_create_gap(size() + __n, __pos, __n);

        _Traits::assign(data() + __pos, __n, __c);

        return *this;
    }

    constexpr iterator insert(const_iterator __p, _CharT __c)
    {
        size_type __offset = __p - begin();
        insert(__offset, __c);
        return begin() + __offset;
    }

    constexpr iterator insert(const_iterator __p, size_type __n, _CharT __c)
    {
        if (__n == 0)
            return __p;

        size_type __offset = __p - begin();
        insert(__offset, __n, __c);
        return begin() + __offset;
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr iterator insert(const_iterator __p, _InputIterator __first, _InputIterator __last)
    {
        if (__p == cend())
            return append(__first, __last);

        if constexpr (std::is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        {
            auto __dist = distance(__first, __last);
            auto __orig_offset = __p - cbegin();

            if (__dist > max_size() - size())
                __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::insert"));

            __ensure_space_and_create_gap(size() + __dist, __p, __dist);

            for (auto __offset = __orig_offset; __first != __last; ++__first, ++__offset)
                _Traits::assign(data() + __offset, static_cast<_CharT>(*__first));

            return begin() + __orig_offset;
        }
        else
        {
            auto __orig_offset = __p - cbegin();

            for (auto __offset = __orig_offset; __first != __last; ++__first, ++__offset)
                insert(cbegin() + __offset, static_cast<_CharT>(*__first));

            return begin() + __orig_offset;
        }
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr iterator insert_range(const_iterator __p, _R&& __r)
    {
        if (__p == cend())
            return append_range(std::forward<_R>(__r));

        if constexpr (ranges::sized_range<_R>)
        {
            auto __dist = ranges::distance(__r);
            auto __offset = __p - cbegin();

            if (__dist > max_size() - size())
                __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::insert_range"));

            __ensure_space_and_create_gap(size() + __dist, __p, __dist);

            for (auto __first = ranges::begin(__r), __last = ranges::end(__r); __first != __last; ++__first, ++__offset)
                _Traits::assign(data() + __offset, static_cast<_CharT>(*__first));
        }
        else
        {
            auto __offset = __p - cbegin();
            for (auto __first = ranges::begin(__r), __last = ranges::end(__r); __first != __last; ++__first, ++__offset)
                insert(cbegin() + __offset, static_cast<_CharT>(*__first));
        }
    }

    constexpr iterator insert(const_iterator __p, initializer_list<_CharT> __il)
    {
        return insert(__p, __il.begin(), __il.end());
    }

    constexpr basic_string& erase(size_type __pos = 0, size_type __n = npos)
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::erase"));

        auto __xlen = min(__n, size() - __pos);
        _Traits::move(data() + __pos, data() + __pos + __xlen, __xlen);

        if (__is_inline())
        {
            _M_inline_length -= __xlen;
            if (_M_inline_length < _MaxInline)
                _M_inline[_M_inline_length] = _CharT(0);
        }
        else
        {
            _M_length -= __xlen;
            _M_memory[_M_length] = _CharT(0);
        }

        return *this;
    }

    constexpr iterator erase(const_iterator __p)
    {
        auto __offset = __p - begin();
        erase(__offset, 1);
        return __p;
    }

    constexpr iterator erase(const_iterator __first, const_iterator __last)
    {
        auto __offset = __first - begin();
        erase(__offset, __last - __first);
        return __first;
    }

    constexpr void pop_back()
    {
        erase(end() - 1);
    }

    constexpr basic_string& replace(size_type __pos1, size_type __n1, const basic_string& __str)
    {
        return replace(__pos1, __n1, __str.data(), __str.size());
    }

    constexpr basic_string& replace(size_type __pos1, size_type __n1, const basic_string&& __str, size_type __pos2, size_type __n2 = npos)
    {
        return replace(__pos1, __n1, basic_string_view<_CharT, _Traits>(__str).substr(__pos2, __n2));
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& replace(size_type __pos1, size_type __n1, const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return replace(__pos1, __n1, __sv.data(), __sv.size());
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& replace(size_type __pos1, size_type __n1, const _T& __t, size_type __pos2, size_type __n2 = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return replace(__pos1, __n1, __sv.substr(__pos2, __n2));
    }

    constexpr basic_string& replace(size_type __pos, size_type __n1, const _CharT* __s, size_type __n2)
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::replace"));
        
        auto __xlen = min(__n1, size() - __pos);
        if (size() - __xlen >= max_size() - __n2)
            __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::replace"));

        size_type __req_size = size() - __xlen + __n2;

        if (capacity() < __req_size)
        {
            // Construct in a temporary string.
            basic_string __str;
            __str.reserve(__req_size);
            __str.append(data(), __pos);
            __str.append(__s, __n2);
            __str.append(data() + __pos + __xlen, size() - __pos - __xlen);
            swap(__str);
        }
        else
        {
            // Shift the data around to make a hole and copy the replacement into the hole.
            _Traits::move(data() + __pos + __n2, data() + __pos + __xlen, size() - __pos - __xlen);
            _Traits::copy(data() + __pos, __s, __n2);
        }

        return *this;
    }

    constexpr basic_string& replace(size_type __pos, size_type __n1, const _CharT* __s)
    {
        return replace(__pos, __n1, __s, _Traits::length(__s));
    }

    constexpr basic_string& replace(size_type __pos, size_type __n1, size_type __n2, _CharT __c)
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::replace"));
        
        auto __xlen = min(__n1, size() - __pos);
        if (size() - __xlen >= max_size() - __n2)
            __XVI_CXX_UTILITY_THROW(length_error("length > max_size() in std::basic_string::replace"));

        size_type __req_size = size() - __xlen + __n2;

        if (capacity() < __req_size)
        {
            // Construct in a temporary string.
            basic_string __str;
            __str.reserve(__req_size);
            __str.append(data(), __pos);
            __str.append(__n2, __c);
            __str.append(data() + __pos + __xlen, size() - __pos - __xlen);
            swap(__str);
        }
        else
        {
            // Shift the data around to make a hole and copy the replacement into the hole.
            _Traits::move(data() + __pos + __n2, data() + __pos + __xlen, size() - __pos - __xlen);
            _Traits::assign(data() + __pos, __n2, __c);
        }

        return *this;
    }

    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, const basic_string& __str)
    {
        return replace(__i1, __i2, basic_string_view<_CharT, _Traits>(__str));
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, const _T& __t)
    {
        basic_string_view <_CharT, _Traits> __sv = __t;
        return replace(__i1 - begin(), __i2 - __i1, __sv.data(), __sv.size());
    }

    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, const _CharT* __s, size_type __n)
    {
        return replace(__i1, __i2, basic_string_view<_CharT, _Traits>(__s, __n));
    }

    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, const _CharT* __s)
    {
        return replace(__i1, __i2, basic_string_view<_CharT, _Traits>(__s));
    }

    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, size_type __n, _CharT __c)
    {
        return replace(__i1 - begin(), __i2 - __i1, __n, __c);
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, _InputIterator __j1, _InputIterator __j2)
    {
        if constexpr (std::is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        {
            auto __dist = distance(__j1, __j2);
            auto __replaced_dist = __i2 - __i1;

            // Will this fit in the current allocation?
            if (capacity() <= (size() - __replaced_dist + __dist))
            {
                // Make a hole for the replacement data.
                auto __hole_source_offset = __i2 - cbegin();
                auto __hole_start_offset = __i1 - cbegin();
                auto __hole_end_offset = __hole_start_offset + __dist;
                _Traits::move(data() + __hole_end_offset, data() + __hole_source_offset, size() - __hole_source_offset);

                for (auto __offset = __hole_start_offset; __j1 != __j2; ++__j1, ++__offset)
                    _Traits::assign(data() + __offset, static_cast<_CharT>(*__j1));
            }
            else
            {
                // Allocation will be required; construct via a temporary.
                basic_string __temp;
                __temp.reserve(size() + __dist - __replaced_dist);
                __temp.append(cbegin(), __i1);
                __temp.append(__j1, __j2);
                __temp.append(__i2, cend());
                swap(__temp);
            }
        }
        else
        {
            // Unknown length. Construct via a temporary copy.
            basic_string __copy;
            __copy.append(cbegin(), __i1);
            __copy.append(__j1, __j2);
            __copy.append(__i2, cend());
            swap(__copy);
        }

        return *this;
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string& replace_with_range(const_iterator __i1, const_iterator __i2, _R&& __rg)
    {
        if constexpr (ranges::sized_range<_R>)
        {
            auto __dist = ranges::distance(std::forward<_R>(__rg));
            auto __replaced_dist = __i2 - __i1;

            // Will this fit in the current allocation?
            if (capacity() <= (size() - __replaced_dist + __dist))
            {
                // Make a hole for the replacement data.
                auto __hole_source_offset = __i2 - cbegin();
                auto __hole_start_offset = __i1 - cbegin();
                auto __hole_end_offset = __hole_start_offset + __dist;
                _Traits::move(data() + __hole_end_offset, data() + __hole_source_offset, size() - __hole_source_offset);

                auto __j1 = ranges::begin(std::forward<_R>(__rg));
                auto __j2 = ranges::end(std::forward<_R>(__rg));
                for (auto __offset = __hole_start_offset; __j1 != __j2; ++__j1, ++__offset)
                    _Traits::assign(data() + __offset, static_cast<_CharT>(*__j1));
            }
            else
            {
                // Allocation will be required; construct via a temporary.
                basic_string __temp;
                __temp.reserve(size() + __dist - __replaced_dist);
                __temp.append(cbegin(), __i1);
                __temp.append_range(std::forward<_R>(__rg));
                __temp.append(__i2, cend());
                swap(__temp);
            }
        }
        else
        {
            // Unknown length. Construct via a temporary copy.
            basic_string __copy;
            __copy.append(cbegin(), __i1);
            __copy.append_range(std::forward<_R>(__rg));
            __copy.append(__i2, cend());
            swap(__copy);
        }

        return *this;
    }

    constexpr basic_string& replace(const_iterator __i1, const_iterator __i2, initializer_list<_CharT> __il)
    {
        return replace(__i1, __i2, __il.begin(), __il.end());
    }

    constexpr size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const
    {
        return __sv().copy(__s, __n, __pos);
    }

    constexpr void swap(basic_string& __str)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_swap::value
                 || allocator_traits<_Allocator>::is_always_equal::value)
    {
        basic_string __temp = std::move(__str);
        __str = std::move(*this);
        *this = std::move(__temp);
    }

    constexpr const _CharT* c_str() const noexcept
    {
        return data();
    }

    constexpr const _CharT* data() const noexcept
    {
        return __is_inline() ? std::addressof(_M_inline[0]) : _M_memory;
    }

    constexpr _CharT* data() noexcept
    {
        return __is_inline() ? std::addressof(_M_inline[0]) : _M_memory;
    }

    constexpr operator basic_string_view<_CharT, _Traits>() const noexcept
    {
        return __sv();
    }

    constexpr allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().find(__st);
    }

    constexpr size_type find(const basic_string& __str, size_type __pos = 0) const noexcept
    {
        return __sv().find(__str, __pos);
    }

    constexpr size_type find(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().find(__s, __pos, __n);
    }

    constexpr size_type find(const _CharT* __s, size_type __pos = 0) const
    {
        return __sv().find(__s, __pos);
    }

    constexpr size_type find(_CharT __c, size_type __pos = 0) const noexcept
    {
        return __sv().find(__c, __pos);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type rfind(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().rfind(__st);
    }

    constexpr size_type rfind(const basic_string& __str, size_type __pos = npos) const noexcept
    {
        return __sv().rfind(__str, __pos);
    }

    constexpr size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().rfind(__s, __pos, __n);
    }

    constexpr size_type rfind(const _CharT* __s, size_type __pos = npos) const
    {
        return __sv().rfind(__s, __pos);
    }

    constexpr size_type rfind(_CharT __c, size_type __pos = npos) const noexcept
    {
        return __sv().rfind(__c, __pos);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_first_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().find_first_of(__st);
    }

    constexpr size_type find_first_of(const basic_string& __str, size_type __pos = 0) const noexcept
    {
        return __sv().find_first_of(__str, __pos);
    }

    constexpr size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().find_first_of(__s, __pos, __n);
    }

    constexpr size_type find_first_of(const _CharT* __s, size_type __pos = 0) const
    {
        return __sv().find_first_of(__s, __pos);
    }

    constexpr size_type find_first_of(_CharT __c, size_type __pos = 0) const noexcept
    {
        return __sv().find_first_of(__c, __pos);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_last_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().find_last_of(__st);
    }

    constexpr size_type find_last_of(const basic_string& __str, size_type __pos = npos) const noexcept
    {
        return __sv().find_last_of(__str, __pos);
    }

    constexpr size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().find_last_of(__s, __pos, __n);
    }

    constexpr size_type find_last_of(const _CharT* __s, size_type __pos = npos) const
    {
        return __sv().find_last_of(__s, __pos);
    }

    constexpr size_type find_last_of(_CharT __c, size_type __pos = npos) const noexcept
    {
        return __sv().find_last_of(__c, __pos);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_first_not_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().find_first_not_of(__st);
    }

    constexpr size_type find_first_not_of(const basic_string& __str, size_type __pos = 0) const noexcept
    {
        return __sv().find_first_not_of(__str, __pos);
    }

    constexpr size_type find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().find_first_not_of(__s, __pos, __n);
    }

    constexpr size_type find_first_not_of(const _CharT* __s, size_type __pos = 0) const
    {
        return __sv().find_first_not_of(__s, __pos);
    }

    constexpr size_type find_first_not_of(_CharT __c, size_type __pos = 0) const noexcept
    {
        return __sv().find_first_not_of(__c, __pos);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_last_not_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        __sv().find_last_not_of(__st);
    }

    constexpr size_type find_last_not_of(const basic_string& __str, size_type __pos = npos) const noexcept
    {
        return __sv().find_last_not_of(__str, __pos);
    }

    constexpr size_type find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        return __sv().find_last_not_of(__s, __pos, __n);
    }

    constexpr size_type find_last_not_of(const _CharT* __s, size_type __pos = npos) const
    {
        return __sv().find_last_not_of(__s, __pos);
    }

    constexpr size_type find_last_not_of(_CharT __c, size_type __pos = npos) const noexcept
    {
        return __sv().find_last_not_of(__c, __pos);
    }

    constexpr basic_string substr(size_type __pos = 0, size_type __n = npos) const
    {
        if (__pos > size())
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid offset for std::basic_string::substr"));

        auto __rlen = min(__n, size() - __pos);
        
        return basic_string(data() + __pos, __rlen);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr int compare(const _T& __t) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        return __sv().compare(__t);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr int compare(size_type __pos1, size_type __n1, const _T& __t) const
    {
        return __sv().substr(__pos1, __n1).compare(__t);
    }

    template <class _T>
        requires std::is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!std::is_convertible_v<const _T&, const _CharT*>)
    constexpr int compare(size_type __pos1, size_type __n1, const _T& __t, size_type __pos2, size_type __n2 = npos) const
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().substr(__pos1, __n1).compare(__st.substr(__pos2, __n2));
    }

    constexpr int compare(const basic_string& __str) const noexcept
    {
        return __sv().compare(__str);
    }

    constexpr int compare(size_type __pos1, size_type __n1, const basic_string& __str) const noexcept
    {
        return __sv().compare(__pos1, __n1, __str);
    }

    constexpr int compare(size_type __pos1, size_type __n1, const basic_string& __str, size_type __pos2, size_type __n2 = npos) const noexcept
    {
        return __sv().compare(__pos1, __n1, __str, __pos2, __n2);
    }

    constexpr int compare(const _CharT* __s) const
    {
        return __sv().compare(__s);
    }

    constexpr int compare(size_type __pos1, size_type __n1, const _CharT* __s) const
    {
        return __sv().compare(__pos1, __n1, __s);
    }

    constexpr int compare(size_type __pos1, size_type __n1, const _CharT* __s, size_type __n2) const
    {
        return __sv().compare(__pos1, __n1, __s, __n2);
    }

    constexpr bool starts_with(basic_string_view<_CharT, _Traits> __x) const noexcept
    {
        return __sv().starts_with(__x);
    }

    constexpr bool starts_with(_CharT __x) const noexcept
    {
        return __sv().starts_with(__x);
    }

    constexpr bool starts_with(const _CharT* __s) const
    {
        return __sv().starts_with(__s);
    }

    constexpr bool ends_with(basic_string_view<_CharT, _Traits> __x) const noexcept
    {
        return __sv().ends_with(__x);
    }

    constexpr bool ends_with(_CharT __x) const noexcept
    {
        return __sv().ends_with(__x);
    }

    constexpr bool ends_with(const _CharT* __s) const
    {
        return __sv().ends_with(__s);
    }

    constexpr bool contains(basic_string_view<_CharT, _Traits> __x) const noexcept
    {
        return __sv().contains(__x);
    }

    constexpr bool contains(_CharT __x) const noexcept
    {
        return __sv().contains(__x);
    }

    constexpr bool contains(const _CharT* __s) const
    {
        return __sv().contains(__s);
    }

private:

    using _Storage::_MaxInline;

    using _Storage::_M_length;
    using _Storage::_M_capacity;
    using _Storage::_M_memory;
    using _Storage::_M_padded_flags;
    using _Storage::_M_inline_length;
    using _Storage::_M_inline;
    using _Storage::_M_flags;

    static constexpr size_t __grow_numerator = 4;
    static constexpr size_t __grow_denomenator = 3;

    [[no_unique_address]] _Allocator _M_allocator;

    constexpr bool __is_inline() const noexcept
    {
        return !is_constant_evaluated() && _M_flags.__value == 0;
    }

    constexpr void __constexpr_setup() noexcept
    {
        if (!is_constant_evaluated())
            return;

        // We never use inline storage when evaluating in constexpr context.
        //
        // This is to avoid some of the issues caused with the way unions are used for storing the data as well as to
        // provide consistent behaviour regardless of string length.
        _M_length = 0;
        _M_capacity = 0;
        _M_memory = nullptr;
        _M_padded_flags.__value = 1;
    }

    constexpr void __mark_inline() noexcept
    {
        if (is_constant_evaluated())
        {
            __constexpr_setup();
            return;
        }
    
        _M_flags.__value = 0;
        _M_inline_length = 0;
        _M_inline[0] = _CharT(0);
    }

    constexpr void __mark_not_inline() noexcept
    {
        if (is_constant_evaluated())
        {   
            __constexpr_setup();
            return;
        }
        
        _M_padded_flags.__value |= 0x01;
        _M_length = _M_capacity = 0;
        _M_memory = nullptr;
    }

    constexpr void __truncate_in_place(size_type __new_length) noexcept
    {
        // If inline, set the terminator and we're done.
        if (__is_inline())
        {
            // Already terminated if __new_length == _MaxInline but, if so, we're not actually truncating so nothing
            // needs to be done in that case.
            //
            // Otherwise, ensure the string is terminated in the right place.
            if (__new_length < _MaxInline)
                _M_inline[__new_length] = _CharT(0);

            _M_inline_length = __new_length;

            return;
        }

        // Set the terminator and update the size.
        _M_memory[__new_length] = _CharT(0);
        _M_length = __new_length;
    }

    constexpr void __deallocate()
    {
        if (__is_inline())
        {
            // Reset for consistency.
            __mark_inline();
            return;
        }

        // Free the associated memory.
        if (_M_memory)
            allocator_traits<_Allocator>::deallocate(_M_allocator, _M_memory, _M_capacity);
        __mark_inline();
    }

    constexpr void __ensure_space(size_type __n)
    {
        if (__n <= capacity())
            return;

        auto __target = (capacity() * __grow_numerator) / __grow_denomenator;
        if (__target > __n)
            __n = __target;

        __reallocate(__n);
    }

    constexpr void __ensure_space_and_create_gap(size_type __n, size_type __gap_pos, size_type __gap_len)
    {
        if (capacity() < __n)
        {
            auto __target = (capacity() * __grow_numerator) / __grow_denomenator;
            if (__target > __n)
                __n = __target;
            
            __reallocate(__n, __gap_pos, __gap_len);
            return;
        }

        _Traits::move(data() + __gap_pos + __gap_len, data() + __gap_pos, __gap_len);

        if (__is_inline())
        {
            _M_inline_length += __gap_len;
            if (_M_inline_length < _MaxInline)
                _M_inline[_M_inline_length] = _CharT(0);
        }
        else
        {
            _M_length += __gap_len;
            _M_memory[_M_length] = _CharT(0);
        }
    }

    constexpr void __reallocate(size_type __req_capacity, size_type __gap_pos = npos, size_type __gap_len = 0)
    {
        // Check for overflow.
        if (__req_capacity > max_size() || __gap_len > __req_capacity)
            __XVI_CXX_UTILITY_THROW(bad_alloc());

        // Check for truncation.
        if (__req_capacity < size() + __gap_len)
            __truncate_in_place(__req_capacity - __gap_len);

        // Handle "problematic" gap positions.
        bool __gap_at_end = false;
        if (__gap_pos == npos || __gap_pos >= size())
        {
            __gap_at_end = true;
            __gap_pos = 0;
        }

        // Can the "allocation" be stored inline?
        if (__req_capacity <= _MaxInline && !is_constant_evaluated())
        {
            // If already inline, all that needs to be done is to create the gap (if any).
            if (__is_inline())
            {
                // Create the gap and update the length.
                if (__gap_len > 0 && !__gap_at_end)
                    _Traits::move(&_M_inline[__gap_pos + __gap_len], &_M_inline[__gap_pos], __gap_len);
                _M_inline_length += __gap_len;

                // Ensure the string stays terminated.
                if (_M_inline_length < _MaxInline)
                    _M_inline[_M_inline_length] = _CharT(0);

                // Nothing more to do for inline strings.
                return;
            }

            // Switching from heap storage to inline.
            _CharT* __current = data();
            size_type __current_size = size();
            size_type __current_capacity = capacity() + 1;

            // Copy the data inline and terminate it.
            __mark_inline();
            _M_inline_length = __current_size;
            _Traits::copy(&_M_inline[0], __current, __current_size);
            if (__current_size < _MaxInline)
                _M_inline[__current_size] = _CharT(0);

            // Free the heap memory.
            if (_M_memory)
                allocator_traits<_Allocator>::deallocate(_M_allocator, __current, __current_capacity);
            
            return;
        }
        
        // Bump the capacity to account for the NUL terminator.
        size_type __new_capacity = __req_capacity + 1;
        
        // Allocate the new memory.
        _CharT* __p = allocator_traits<_Allocator>::allocate(_M_allocator, __new_capacity);
        if (!__p)
            __XVI_CXX_UTILITY_THROW(bad_alloc());

        _CharT* __current = data();
        size_type __current_size = size();
        size_type __current_capacity = capacity() + 1;
        bool __currently_inline = __is_inline();

        __XVI_CXX_UTILITY_TRY
        {
            // Copy all data up to the first gap.
            if (__gap_pos != 0)
                _Traits::copy(__p, __current, __gap_pos);

            // Copy all data after the gap.
            _Traits::copy(__p + __gap_pos + __gap_len, __current + __gap_pos, __current_size - __gap_pos);

            // Set the NUL terminator.
            __p[__new_capacity - 1] = _CharT(0);
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if (_M_memory)
                allocator_traits<_Allocator>::deallocate(_M_allocator, __p, __new_capacity);
            __XVI_CXX_UTILITY_RETHROW;
        }

        // Update the length values and data pointer.
        __mark_not_inline();
        _M_length = __current_size + __gap_len;
        _M_capacity = __req_capacity;
        _M_memory = __p;

        // Free the old memory.
        if (!__currently_inline && __current)
            allocator_traits<_Allocator>::deallocate(_M_allocator, __current, __current_capacity);
    }

    constexpr basic_string_view<_CharT, _Traits> __sv() const noexcept
    {
        return basic_string_view<_CharT, _Traits>(data(), size());
    }
};

// Sanity check.
static_assert(sizeof(basic_string<char>) == 4 * sizeof(void*));
static_assert(sizeof(basic_string<char16_t>) == 4*sizeof(void*));
static_assert(sizeof(basic_string<char32_t>) == 4*sizeof(void*));

template <class _InputIterator,
          class _Allocator = allocator<typename iterator_traits<_InputIterator>::value_type>>
    requires __detail::__maybe_iterator<_InputIterator> && __detail::__maybe_allocator<_Allocator>
basic_string(_InputIterator, _InputIterator, _Allocator = _Allocator())
    -> basic_string<typename iterator_traits<_InputIterator>::value_type,
                    char_traits<typename iterator_traits<_InputIterator>::value_type>,
                    _Allocator>;

template <ranges::input_range _R,
          class _Allocator = allocator<ranges::range_value_t<_R>>>
basic_string(from_range_t, _R&&, _Allocator = _Allocator())
    -> basic_string<ranges::range_value_t<_R>, char_traits<ranges::range_value_t<_R>>, _Allocator>;

template <class _CharT, class _Traits, class _Allocator = allocator<_CharT>>
    requires __detail::__maybe_allocator<_Allocator>
explicit basic_string(basic_string_view<_CharT, _Traits>, const _Allocator& = _Allocator())
    -> basic_string<_CharT, _Traits, _Allocator>;

template <class _CharT, class _Traits, class _Allocator = allocator<_CharT>>
    requires __detail::__maybe_allocator<_Allocator>
basic_string(basic_string_view<_CharT, _Traits>,
             typename basic_string<_CharT, _Traits, _Allocator>::size_type,
             typename basic_string<_CharT, _Traits, _Allocator>::size_type,
             const _Allocator& = _Allocator())
    -> basic_string<_CharT, _Traits, _Allocator>;


template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
{
    basic_string<_CharT, _Traits, _Alloc> __r;
    __r.reserve(__lhs.size() + __rhs.size());
    __r.append(__lhs).append(__rhs);
    return __r;
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const _CharT* __rhs)
{
    auto __rhs_len = _Traits::length(__rhs);
    
    basic_string<_CharT, _Traits, _Alloc> __r;
    __r.reserve(__lhs.size() + char_traits<_CharT>::length(__rhs));
    __r.append(__lhs).append(__rhs, __rhs_len);
    return __r;
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
{
    __lhs.append(__rhs);
    return std::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, const _CharT* __rhs)
{
    __lhs.append(__rhs);
    return std::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, basic_string<_CharT, _Traits, _Alloc>&& __rhs)
{
    auto __req_size = __lhs.size() + __rhs.size();
    if (__lhs.get_allocator() == __rhs.get_allocator() && __rhs.capacity() >= __req_size)
    {
        __rhs.insert(0, __lhs);
        return std::move(__rhs);
    }
    
    __lhs.append(__rhs);
    return std::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, basic_string<_CharT, _Traits, _Alloc>&& __rhs)
{
    __rhs.insert(0, __lhs);
    return std::move(__rhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const _CharT* __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
{
    auto __lhs_len = _Traits::length(__lhs);
   
    basic_string<_CharT, _Traits, _Alloc> __r;
    __r.reserve(__lhs_len + __rhs.size());
    __r.append(__lhs, __lhs_len).append(__rhs);
    return __r;
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(_CharT __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
{
    basic_string<_CharT, _Traits, _Alloc> __r;
    __r.reserve(1 + __rhs.size());
    __r.append(1, __lhs).append(__rhs);
    return __r;
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(_CharT __lhs, basic_string<_CharT, _Traits, _Alloc>&& __rhs)
{
    __rhs.insert(__rhs.begin(), __lhs);
    return std::move(__rhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, _CharT __rhs)
{
    basic_string<_CharT, _Traits, _Alloc> __r;
    __r.reserve(__lhs.size() + 1);
    __r.append(__lhs).append(1, __rhs);
    return __r;
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, _CharT __rhs)
{
    __lhs.push_back(__rhs);
    return std::move(__lhs);
}


template <class _CharT, class _Traits, class _Alloc>
constexpr bool operator==(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs) noexcept
{
    return basic_string_view<_CharT, _Traits>(__lhs) == basic_string_view<_CharT, _Traits>(__rhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr bool operator==(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const _CharT* __rhs) noexcept
{
    return basic_string_view<_CharT, _Traits>(__lhs) == basic_string_view<_CharT, _Traits>(__rhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr auto operator<=>(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
{
    return basic_string_view<_CharT, _Traits>(__lhs) <=> basic_string_view<_CharT, _Traits>(__rhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr auto operator<=>(const basic_string<_CharT, _Traits, _Alloc>& __lhs, const _CharT* __rhs)
{
    return basic_string_view<_CharT, _Traits>(__lhs) <=> basic_string_view<_CharT, _Traits>(__rhs);
}


template <class _CharT, class _Traits, class _Alloc>
void swap(basic_string<_CharT, _Traits, _Alloc>& __lhs, basic_string<_CharT, _Traits, _Alloc>& __rhs)
    noexcept(noexcept(__lhs.swap(__rhs)))
{
    __lhs.swap(__rhs);
}


//! @TODO: iostreams


template <class _CharT, class _Traits, class _Alloc, class _U>
constexpr typename basic_string<_CharT, _Traits, _Alloc>::size_type
erase(basic_string<_CharT, _Traits, _Alloc>& __c, const _U& __value)
{
    auto __it = remove(__c.begin(), __c.end(), __value);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());
    return __r;
}

template <class _CharT, class _Traits, class _Alloc, class _Predicate>
constexpr typename basic_string<_CharT, _Traits, _Alloc>::size_type
erase_if(basic_string<_CharT, _Traits, _Alloc>& __c, _Predicate __pred)
{
    auto __it = remove_if(__c.begin(), __c.end(), __pred);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());
    return __r;
}


//! @TODO: conversions


inline namespace literals
{
inline namespace string_literals
{

// GCC doesn't always detect this as a system header properly and throws warnings about these suffixes.
#pragma GCC diagnostic push
#ifdef __llvm__
#  pragma GCC diagnostic ignored "-Wuser-defined-literals"
#else
#  pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif

constexpr string operator""s(const char* __str, size_t __len)
{
    return string{__str, __len};
}

constexpr u8string operator""s(const char8_t* __str, size_t __len)
{
    return u8string{__str, __len};
}

constexpr u16string operator""s(const char16_t* __str, size_t __len)
{
    return u16string{__str, __len};
}

constexpr u32string operator""s(const char32_t* __str, size_t __len)
{
    return u32string{__str, __len};
}

constexpr wstring operator""s(const wchar_t* __str, size_t __len)
{
    return wstring{__str, __len};
}

#pragma GCC diagnostic pop

} // namespace string_literals
} // namespace literals

} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_STRING_H */

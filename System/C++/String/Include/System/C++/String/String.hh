#ifndef __SYSTEM_CXX_STRING_STRING_H
#define __SYSTEM_CXX_STRING_STRING_H


#include <System/C++/Allocator/Allocator.hh>
#include <System/C++/Allocator/AllocatorTraits.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/IteratorAdaptors.hh>
#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>

#include <System/C++/String/Private/Config.hh>
#include <System/C++/String/CharTraits.hh>
#include <System/C++/String/StringFwd.hh>
#include <System/C++/String/StringView.hh>


//! @todo: check that allocators are handled correctly on copy & move assignments (and on swaps).


namespace __XVI_STD_STRING_NS_DECL
{


namespace __detail
{


template <class _CharT, class _SizeType>
class __basic_string_storage
{
public:
    
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
        make_unsigned_t<_CharT>         __value = 0;
    };

    struct __unpadded_flags_t
    {
        make_unsigned_t<_CharT>         __value = 0;
    };

    template <size_t _Padding = (sizeof(void*) - sizeof(_CharT)) / sizeof(_CharT)>
    using __flags_t = conditional_t<_Padding == 0, __unpadded_flags_t, __padded_flags_t<_Padding>>;

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

    constexpr _CharT* __data() noexcept
    {
        return __is_inline() ? __XVI_STD_NS::addressof(_M_inline[0]) : _M_memory;
    }

    constexpr const _CharT* __data() const noexcept
    {
        return __is_inline() ? __XVI_STD_NS::addressof(_M_inline[0]) : _M_memory;
    }

    constexpr _SizeType __size() const noexcept
    {
        return __is_inline() ? static_cast<_SizeType>(_M_inline_length) : _M_length;
    }

    constexpr _SizeType __capacity() const noexcept
    {
        return __is_inline() ? _MaxInline : _M_capacity;
    }
};


[[noreturn]] void __string_bad_alloc();
[[noreturn]] void __string_length_error(const char*);
[[noreturn]] void __string_out_of_range(const char*);

[[noreturn]] void __string_precondition_failed(const char*);


} // namespace __detail


// Forward declarations.
template <class, class> class basic_string_view;


template <class _CharT, class _Traits, class _Allocator>
class basic_string :
    private __detail::__basic_string_storage<_CharT, typename allocator_traits<_Allocator>::size_type>
{
    template <class, class, class> friend class basic_string;

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
    using reverse_iterator          = __XVI_STD_NS::reverse_iterator<iterator>;
    using const_reverse_iterator    = __XVI_STD_NS::reverse_iterator<const_iterator>;

    using _Storage = __detail::__basic_string_storage<_CharT, size_type>;

    static constexpr size_type npos = static_cast<size_type>(-1);

    constexpr basic_string() noexcept(noexcept(_Allocator()))
        : basic_string(_Allocator())
    {
    }

    constexpr explicit basic_string(const _Allocator& __a) noexcept
        : _Storage{},
          _M_allocator(allocator_traits<_Allocator>::select_on_container_copy_construction(__a))
    {
        __constexpr_setup();
    }

    constexpr basic_string(const basic_string& __s)
        : basic_string(__s.get_allocator())
    {
        append(__s);
    }

    constexpr basic_string(basic_string&& __s) noexcept
        : _M_allocator(__XVI_STD_NS::move(__s._M_allocator))
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

    constexpr basic_string(basic_string&& __s, size_type __pos, const _Allocator& __a = _Allocator())
        : basic_string(__s, __pos, npos, __a)
    {
    }

    constexpr basic_string(basic_string&& __s, size_type __pos, size_type __n, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        if (__pos > __s.size())
            __throw_out_of_range("invalid string index");

        auto __rlen = __pos + __min(__n, __s.size() - __pos);

        if (allocator_traits<_Allocator>::is_always_equal::value || _M_allocator == __s.get_allocator())
        {
            if (__s.__is_inline())
            {
                assign(basic_string_view<_CharT, _Traits>(__s.data() + __pos, __s.data() + __rlen));
            }
            else
            {
                // Move the memory then perform the substring operation.
                __mark_not_inline();
                _M_length = __s._M_length;
                _M_capacity = __s._M_capacity;
                _M_memory = __s._M_memory;
                __s.__mark_inline();

                auto __len = __rlen - __pos;
                _Traits::move(data(), data() + __pos, __len);
                __truncate_in_place(__len);
            }
        }
        else
        {
            // Allocators differ - can't move the memory.
            assign(basic_string_view<_CharT, _Traits>(__s.data() + __pos, __s.data() + __rlen));
        }
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
    constexpr basic_string(const _T& __t, size_type __pos, size_type __n, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        operator=(basic_string(__sv.substr(__pos, __n), __a));
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string(const _T& __t, const _Allocator& __a = _Allocator())
        : basic_string(__a)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        operator=(basic_string(__sv.data(), __sv.size(), __a));
    }

    constexpr basic_string(const _CharT* __s, size_type __n, const _Allocator& __a = _Allocator())
        requires __detail::__maybe_allocator<_Allocator>
        : basic_string(__a)
    {
        if (__s == nullptr && __n != 0) [[unlikely]]
            __precondition_failed("string constructed from null pointer");

        append(__s, __n);
    }

    basic_string(nullptr_t) = delete;

    constexpr basic_string(const _CharT* __s, const _Allocator& __a = _Allocator())
        requires __detail::__maybe_allocator<_Allocator>
        : basic_string(__a)
    {
        if (__s == nullptr) [[unlikely]]
            __precondition_failed("string constructed from null pointer");

        append(__s);
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
        assign_range(__XVI_STD_NS::forward<_R>(__rg));
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
        if (__XVI_STD_NS::addressof(__s) == this)
            return *this;
        
        if (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
            && (!allocator_traits<_Allocator>::is_always_equal::value && _M_allocator != __s.get_allocator()))
        {
            __deallocate();
            _M_allocator = __s.get_allocator();
            return append(__s);
        }

        clear();
        return append(__s);
    }

    constexpr basic_string& operator=(basic_string&& __s)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                 || allocator_traits<_Allocator>::is_always_equal::value)
    {
        if (__XVI_STD_NS::addressof(__s) == this)
            return *this;

        if (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
            || allocator_traits<_Allocator>::is_always_equal::value
            || _M_allocator == __s.get_allocator())
        {
            __deallocate();

            if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value)
                _M_allocator = __XVI_STD_NS::move(__s._M_allocator);

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
        }
        else
        {
            // Can't move memory; treat as a copy.
            clear();
            append(__s);
        }

        return *this;
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& operator=(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv);
    }

    constexpr basic_string& operator=(const _CharT* __s)
    {
        if (__s == nullptr) [[unlikely]]
            __precondition_failed("null pointer assigned to string");

        return *this = basic_string_view<_CharT, _Traits>(__s);
    }

    basic_string& operator=(nullptr_t) = delete;

    constexpr basic_string& operator=(_CharT __c)
    {
        return *this = basic_string_view<_CharT, _Traits>(__XVI_STD_NS::addressof(__c), 1);
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
        return reverse_iterator(begin());
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
        return __size();
    }

    constexpr size_type length() const noexcept
    {
        return size();
    }

    constexpr size_type max_size() const noexcept
    {
        return __max_size();
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
        auto __k = __min(__o, __n);
        auto __g = __n - __k;

        __set_storage(__reallocate(__n, __k, __g));

        try
        {
            auto __m = __n;
            auto __p = data();
            auto __r = __XVI_STD_NS::move(__op)(__p, __m);

            if (__p != data() || __m != __n || __r < 0 || __r > __n) [[unlikely]]
                __precondition_failed("operation failed preconditions during string resize_and_overwrite");

            __truncate_in_place(__r);
        }
        catch (...)
        {
            __precondition_failed("exception thrown during string resize_and_overwrite");
        }
    }

    constexpr size_type capacity() const noexcept
    {
        return __capacity();
    }

    constexpr void reserve(size_type __n)
    {
        if (__n > max_size())
            __throw_length_error("std::basic_string::reserve(n) with n > max_size()");

        if (capacity() < __n)
            __set_storage(__reallocate(__n));
    }

    constexpr void shrink_to_fit()
    {
        __set_storage(__reallocate(size()));
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
        if (__pos > size())
            __precondition_failed("invalid index for std::basic_string");        

        return data()[__pos];
    }
    
    constexpr reference operator[](size_type __pos)
    {
        if (__pos > size())
            __precondition_failed("invalid index for std::basic_string");        

        return data()[__pos];
    }

    constexpr const_reference at(size_type __pos) const
    {
        if (__pos >= size())
            __throw_out_of_range("invalid index for std::basic_string::at");

        return operator[](__pos);
    }

    constexpr reference at(size_type __pos)
    {
        if (__pos >= size())
            __throw_out_of_range("invalid index for std::basic_string::at");

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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& append(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return append(__sv.data(), __sv.size());
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& append(const _T& __t, size_type __pos, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return append(__sv.substr(__pos, __n));
    }

    constexpr basic_string& append(const _CharT* __s, size_type __n)
    {
        if (__s == nullptr && __n != 0)
            __precondition_failed("can't append null C string");

        auto __storage = __ensure_space(size() + __n);

        _Traits::copy(__storage.__data() + __storage.__size(), __s, __n);

        __set_storage(__storage);

        if (__is_inline())
        {
            _M_inline_length += __n;
            if (_M_inline_length < _Storage::_MaxInline)
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
        if (__s == nullptr) [[unlikely]]
            __precondition_failed("can't append null C string");

        return append(__s, _Traits::length(__s));
    }

    constexpr basic_string& append(size_type __n, _CharT __c)
    {
        __set_storage(__ensure_space(size() + __n));
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
        if constexpr (is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        {
            auto __dist = distance(__first, __last);
            auto __needed = size() + __dist;

            auto __storage = __ensure_space(__needed);

            auto __ptr = __storage.__data() + size();
            for (; __first != __last; ++__first, (void)++__ptr)
                _Traits::assign(*__ptr, *__first);

            __set_storage(__storage);

            if (__is_inline())
            {
                _M_inline_length += __dist;
                if (_M_inline_length < _Storage::_MaxInline)
                    _M_inline[_M_inline_length] = _CharT(0);
            }
            else
            {
                _M_length += __dist;
                _M_memory[_M_length] = _CharT(0);
            }

            return *this;
        }
        else
        {
            auto __storage = __ensure_storage(capacity());          

            for (; __first != __last; ++__first)
            {
                // Allocate another character.
                __set_storage(__storage, __ensure_storage(__storage, __storage.__size() + 1));

                auto __ptr = __storage.data() + __storage.size();
                _Traits::assign(*__ptr, *__first);

                if (__storage.__is_inline())
                {
                    __storage._M_inline_length += 1;
                    if (__storage._M_inline_length < _Storage::_MaxInline)
                        __storage._M_inline[__storage._M_inline_length] = _CharT(0);
                }
                else
                {
                    __storage._M_length += 1;
                    __storage._M_memory[__storage._M_length] = _CharT(0);
                }
            }

            __set_storage(__storage);

            return *this;
        }
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string& append_range(_R&& __r)
    {
        if constexpr (ranges::sized_range<_R>)
        {
            auto __dist = ranges::distance(__r);
            auto __needed = size() + __dist;

            auto __storage = __ensure_space(__needed);

            auto __ptr = __storage.__data() + size();
            auto __first = ranges::begin(__XVI_STD_NS::forward<_R>(__r));
            auto __last = ranges::end(__XVI_STD_NS::forward<_R>(__r));

            for (; __first != __last; ++__first, (void)++__ptr)
                _Traits::assign(*__ptr, *__first);

            __set_storage(__storage);

            if (__is_inline())
            {
                _M_inline_length += __dist;
                if (_M_inline_length < _Storage::_MaxInline)
                    _M_inline[_M_inline_length] = _CharT(0);
            }
            else
            {
                _M_length += __dist;
                _M_memory[_M_length] = _CharT(0);
            }

            return *this;
        }
        else
        {
            auto __storage = __ensure_storage(capacity());          

            auto __first = ranges::begin(__XVI_STD_NS::forward<_R>(__r));
            auto __last = ranges::end(__XVI_STD_NS::forward<_R>(__r));
            for (; __first != __last; ++__first)
            {
                // Allocate another character.
                __set_storage(__storage, __ensure_storage(__storage, __storage.__size() + 1));

                auto __ptr = __storage.data() + __storage.size();
                _Traits::assign(*__ptr, *__first);

                if (__storage.__is_inline())
                {
                    __storage._M_inline_length += 1;
                    if (__storage._M_inline_length < _Storage::_MaxInline)
                        __storage._M_inline[__storage._M_inline_length] = _CharT(0);
                }
                else
                {
                    __storage._M_length += 1;
                    __storage._M_memory[__storage._M_length] = _CharT(0);
                }
            }

            __set_storage(__storage);

            return *this;
        }
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
        return *this = __XVI_STD_NS::move(__s);
    }

    constexpr basic_string& assign(const basic_string& __s, size_type __pos, size_type __n = npos)
    {
        return assign(basic_string_view<_CharT, _Traits>(__s).substr(__pos, __n));
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& assign(const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv.data(), __sv.size());
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& assign(const _T& __t, size_type __pos, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return assign(__sv.substr(__pos, __n));
    }

    constexpr basic_string& assign(const _CharT* __s, size_type __n)
    {
        if (__s == nullptr && __n != 0) [[unlikely]]
            __precondition_failed("can't assign null C string");

        auto __storage = __ensure_space(__n, false);
        
        _Traits::copy(__storage.__data(), __s, __n);

        __set_storage(__storage);

        if (__is_inline())
        {
            _M_inline_length = __n;
            if (_M_inline_length < _Storage::_MaxInline)
                _M_inline[_M_inline_length] = _CharT(0);
        }
        else
        {
            _M_length = __n;
            _M_memory[_M_length] = _CharT(0);
        }

        return *this;
    }

    constexpr basic_string& assign(const _CharT* __s)
    {
        if (__s == nullptr) [[unlikely]]
            __precondition_failed("can't assign null C string");        

        return assign(__s, _Traits::length(__s));
    }

    constexpr basic_string& assign(size_type __n, _CharT __c)
    {
        __set_storage(__ensure_space(__n, false));
        __set_zero_length();
        return append(__n, __c);
    }

    template <class _InputIterator>
        requires __detail::__maybe_iterator<_InputIterator>
    constexpr basic_string& assign(_InputIterator __first, _InputIterator __last)
    {
        __set_zero_length();
        append(__first, __last);
        return *this;
    }

    template <__detail::__container_compatible_range<_CharT> _R>
    constexpr basic_string& assign_range(_R&& __r)
    {
        __set_zero_length();
        append_range(__XVI_STD_NS::forward<_R>(__r));
        return *this;
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& insert(size_type __pos, const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return insert(__pos, __sv.data(), __sv.size());
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& insert(size_type __pos, const _T& __t, size_type __pos2, size_type __n = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return insert(__pos, __sv.substr(__pos2, __n));
    }

    constexpr basic_string& insert(size_type __pos, const _CharT* __s, size_type __n)
    {
        if (__pos > size())
            __throw_out_of_range("invalid offset for std::basic_string::insert");

        if (__n > max_size() - size())
            __throw_length_error("length > max_size() in std::basic_string::insert");

        auto __storage = __ensure_space_and_create_gap(size() + __n, __pos, __n);

        _Traits::copy(__storage.__data() + __pos, __s, __n);

        __set_storage(__storage);

        return *this;
    }

    constexpr basic_string& insert(size_type __pos, const _CharT* __s)
    {
        return insert(__pos, __s, _Traits::length(__s));
    }

    constexpr basic_string& insert(size_type __pos, size_type __n, _CharT __c)
    {
        if (__pos > size())
            __throw_out_of_range("invalid offset for std::basic_string::insert");

        if (__n > max_size() - size())
            __throw_length_error("length > max_size() in std::basic_string::insert");

        auto __storage = __ensure_space_and_create_gap(size() + __n, __pos, __n);

        _Traits::assign(__storage.__data() + __pos, __n, __c);

        __set_storage(__storage);

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

        if constexpr (is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        {
            auto __dist = distance(__first, __last);
            auto __orig_offset = __p - cbegin();

            if (__dist > max_size() - size())
                __throw_length_error("length > max_size() in std::basic_string::insert");

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
            return append_range(__XVI_STD_NS::forward<_R>(__r));

        if constexpr (ranges::sized_range<_R>)
        {
            auto __dist = ranges::distance(__r);
            auto __offset = __p - cbegin();

            if (__dist > max_size() - size())
                __throw_length_error("length > max_size() in std::basic_string::insert_range");

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
            __throw_out_of_range("invalid offset for std::basic_string::erase");

        auto __xlen = __min(__n, size() - __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& replace(size_type __pos1, size_type __n1, const _T& __t)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return replace(__pos1, __n1, __sv.data(), __sv.size());
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr basic_string& replace(size_type __pos1, size_type __n1, const _T& __t, size_type __pos2, size_type __n2 = npos)
    {
        basic_string_view<_CharT, _Traits> __sv = __t;
        return replace(__pos1, __n1, __sv.substr(__pos2, __n2));
    }

    constexpr basic_string& replace(size_type __pos, size_type __n1, const _CharT* __s, size_type __n2)
    {
        if (__pos > size())
            __throw_out_of_range("invalid offset for std::basic_string::replace");
        
        auto __xlen = __min(__n1, size() - __pos);
        if (size() - __xlen >= max_size() - __n2)
            __throw_length_error("length > max_size() in std::basic_string::replace");

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
            __throw_out_of_range("invalid offset for std::basic_string::replace");
        
        auto __xlen = __min(__n1, size() - __pos);
        if (size() - __xlen >= max_size() - __n2)
            __throw_length_error("length > max_size() in std::basic_string::replace");

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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
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
        if constexpr (is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
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
            auto __dist = ranges::distance(__XVI_STD_NS::forward<_R>(__rg));
            auto __replaced_dist = __i2 - __i1;

            // Will this fit in the current allocation?
            if (capacity() <= (size() - __replaced_dist + __dist))
            {
                // Make a hole for the replacement data.
                auto __hole_source_offset = __i2 - cbegin();
                auto __hole_start_offset = __i1 - cbegin();
                auto __hole_end_offset = __hole_start_offset + __dist;
                _Traits::move(data() + __hole_end_offset, data() + __hole_source_offset, size() - __hole_source_offset);

                auto __j1 = ranges::begin(__XVI_STD_NS::forward<_R>(__rg));
                auto __j2 = ranges::end(__XVI_STD_NS::forward<_R>(__rg));
                for (auto __offset = __hole_start_offset; __j1 != __j2; ++__j1, ++__offset)
                    _Traits::assign(data() + __offset, static_cast<_CharT>(*__j1));
            }
            else
            {
                // Allocation will be required; construct via a temporary.
                basic_string __temp;
                __temp.reserve(size() + __dist - __replaced_dist);
                __temp.append(cbegin(), __i1);
                __temp.append_range(__XVI_STD_NS::forward<_R>(__rg));
                __temp.append(__i2, cend());
                swap(__temp);
            }
        }
        else
        {
            // Unknown length. Construct via a temporary copy.
            basic_string __copy;
            __copy.append(cbegin(), __i1);
            __copy.append_range(__XVI_STD_NS::forward<_R>(__rg));
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
        basic_string __temp = __XVI_STD_NS::move(__str);
        __str = __XVI_STD_NS::move(*this);
        *this = __XVI_STD_NS::move(__temp);
    }

    constexpr const _CharT* c_str() const noexcept
    {
        return data();
    }

    constexpr const _CharT* data() const noexcept
    {
        return __data();
    }

    constexpr _CharT* data() noexcept
    {
        return __data();
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().find(__st, __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type rfind(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().rfind(__st, __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_first_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().find_first_of(__st, __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_last_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().find_last_of(__st, __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_first_not_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().find_first_not_of(__st, __pos);
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
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr size_type find_last_not_of(const _T& __t, size_type __pos = 0) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        basic_string_view<_CharT, _Traits> __st = __t;
        return __sv().find_last_not_of(__st, __pos);
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
            __throw_out_of_range("invalid offset for std::basic_string::substr");

        auto __rlen = __min(__n, size() - __pos);
        
        return basic_string(data() + __pos, __rlen);
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr int compare(const _T& __t) const
        noexcept(is_nothrow_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>)
    {
        return __sv().compare(__t);
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
    constexpr int compare(size_type __pos1, size_type __n1, const _T& __t) const
    {
        return __sv().substr(__pos1, __n1).compare(__t);
    }

    template <class _T>
        requires is_convertible_v<const _T&, basic_string_view<_CharT, _Traits>>
            && (!is_convertible_v<const _T&, const _CharT*>)
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

    using _Storage::__is_inline;
    using _Storage::__mark_inline;
    using _Storage::__mark_not_inline;
    using _Storage::__constexpr_setup;
    using _Storage::__data;
    using _Storage::__size;
    using _Storage::__capacity;

    static constexpr size_t __grow_numerator = 4;
    static constexpr size_t __grow_denomenator = 3;

    [[no_unique_address]] _Allocator _M_allocator;

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

    constexpr void __deallocate(_Storage& __storage)
    {
        if (__storage.__is_inline())
        {
            // Reset for consistency.
            __storage.__mark_inline();
            return;
        }

        // Free the associated memory.
        if (_M_memory)
            allocator_traits<_Allocator>::deallocate(_M_allocator, __storage._M_memory, (__storage._M_capacity + 1) * sizeof(_CharT));
        __storage.__mark_inline();
    }

    constexpr void __deallocate()
    {
        __deallocate(*static_cast<_Storage*>(this));
    }

    [[nodiscard]] constexpr _Storage __ensure_space(_Storage& __prev, size_type __n, bool __preserve = true)
    {
        if (__n <= __prev.__capacity())
            return *static_cast<_Storage*>(this);

        auto __target = (__prev.__capacity() * __grow_numerator) / __grow_denomenator;
        if (__target > __n)
            __n = __target;

        //! @todo: don't preserve contents when not needed.
        return __reallocate(__prev, __n);
    }

    [[nodiscard]] constexpr _Storage __ensure_space(size_type __n, bool __preserve = true)
    {
        return __ensure_space(*static_cast<_Storage*>(this), __n, __preserve);
    }

    [[nodiscard]] constexpr _Storage __ensure_space_and_create_gap(_Storage& __prev, size_type __n, size_type __gap_pos, size_type __gap_len)
    {
        if (__prev.__capacity() < __n)
        {
            auto __target = (__prev.__capacity() * __grow_numerator) / __grow_denomenator;
            if (__target > __n)
                __n = __target;
            
            return __reallocate(__prev, __n, __gap_pos, __gap_len);
        }

        _Traits::move(__prev.__data() + __gap_pos + __gap_len, __prev.__data() + __gap_pos, __gap_len);

        if (__prev.__is_inline())
        {
            __prev._M_inline_length += __gap_len;
            if (__prev._M_inline_length < _MaxInline)
                __prev._M_inline[__prev._M_inline_length] = _CharT(0);
        }
        else
        {
            __prev._M_length += __gap_len;
            __prev._M_memory[__prev._M_length] = _CharT(0);
        }

        return __prev;
    }

    [[nodiscard]] constexpr _Storage __ensure_space_and_create_gap(size_type __n, size_type __gap_pos, size_type __gap_len)
    {
        return __ensure_space_and_create_gap(*static_cast<_Storage*>(this), __n, __gap_pos, __gap_len);
    }

    [[nodiscard]] constexpr _Storage __reallocate(_Storage& __prev, size_type __req_capacity, size_type __gap_pos = npos, size_type __gap_len = 0)
    {
        // Check for overflow.
        if (__req_capacity > __max_size())
            __throw_length_error("requested size greater than string max_size");

        if (__gap_len > __req_capacity)
            __precondition_failed("string internal error");

        // Check for truncation.
        if (__req_capacity < __prev.__size() + __gap_len)
            __truncate_in_place(__req_capacity - __gap_len);

        // Handle "problematic" gap positions.
        bool __gap_at_end = false;
        if (__gap_pos == npos || __gap_pos >= __prev.__size())
        {
            __gap_at_end = true;
            __gap_pos = __prev.__size();
        }

        // Can the "allocation" be stored inline?
        if (__req_capacity <= _MaxInline && !is_constant_evaluated())
        {
            // If already inline, all that needs to be done is to create the gap (if any).
            if (__prev.__is_inline())
            {
                // Create the gap and update the length.
                if (__gap_len > 0 && !__gap_at_end)
                    _Traits::move(&__prev._M_inline[__gap_pos + __gap_len], &__prev._M_inline[__gap_pos], __gap_len);
                __prev._M_inline_length += __gap_len;

                // Ensure the string stays terminated.
                if (__prev._M_inline_length < _MaxInline)
                    __prev._M_inline[__prev._M_inline_length] = _CharT(0);

                // Nothing more to do for inline strings.
                return __prev;
            }

            // Switching from heap storage to inline.
            _CharT* __current = __prev.__data();
            size_type __current_size = __prev.__size();

            // Copy the data inline and terminate it.
            _Storage __storage;
            __storage.__mark_inline();
            __storage._M_inline_length = __current_size;
            _Traits::copy(&__storage._M_inline[0], __current, __current_size);
            if (__current_size < _MaxInline)
                __storage._M_inline[__current_size] = _CharT(0);

            return __storage;
        }
        
        // Bump the capacity to account for the NUL terminator.
        size_type __new_capacity = __req_capacity + 1;
        
        // Allocate the new memory.
        _CharT* __p = allocator_traits<_Allocator>::allocate(_M_allocator, __new_capacity);
        if (!__p)
            __throw_bad_alloc();

        _CharT* __current = __prev.__data();
        size_type __current_size = __prev.__size();

        try
        {
            // Copy all data up to the start of the gap.
            _Traits::copy(__p, __current, __gap_pos);

            // Copy all data after the gap.
            if (!__gap_at_end)
                _Traits::copy(__p + __gap_pos + __gap_len, __current + __gap_pos, __current_size - __gap_pos);

            // Set the NUL terminator.
            __p[__current_size + __gap_len] = _CharT(0);
        }
        catch (...)
        {
            allocator_traits<_Allocator>::deallocate(_M_allocator, __p, __new_capacity);
            throw;
        }

        // Update the length values and data pointer.
        _Storage __storage;
        __storage.__mark_not_inline();
        __storage._M_length = __current_size + __gap_len;
        __storage._M_capacity = __req_capacity;
        __storage._M_memory = __p;

        return __storage;
    }

    [[nodiscard]] constexpr _Storage __reallocate(size_type __req_capacity, size_type __gap_pos = npos, size_type __gap_len = 0)
    {
        return __reallocate(*static_cast<_Storage*>(this), __req_capacity, __gap_pos, __gap_len);
    }

    constexpr void __set_storage(_Storage& __dest, const _Storage& __storage)
    {
        if (!__dest.__is_inline() && !__storage.__is_inline() && __dest._M_memory == __storage._M_memory)
            return;

        __deallocate(__dest);

        __dest.operator=(__storage);
    }

    constexpr void __set_storage(const _Storage& __storage)
    {
        __set_storage(*static_cast<_Storage*>(this), __storage);
    }

    constexpr void __set_zero_length() noexcept
    {
        if (__is_inline())
        {
            _M_inline_length = 0;
        }
        else
        {
            _M_length = 0;
        }
    }

    constexpr basic_string_view<_CharT, _Traits> __sv() const noexcept
    {
        return basic_string_view<_CharT, _Traits>(data(), size());
    }


    static constexpr size_type __max_size() noexcept
    {
        // Divided by an additional constant factor to prevent overflow when resizing.
        return numeric_limits<size_type>::max() / (sizeof(_CharT) * 4);
    }

    static constexpr size_type __min(size_type __a, size_type __b) noexcept
    {
        return __a < __b ? __a : __b;
    }
    
    [[noreturn]] static void __throw_bad_alloc()
    {
        __detail::__string_bad_alloc();
    }

    [[noreturn]] static void __throw_length_error(const char* __msg)
    {
        __detail::__string_length_error(__msg);
    }

    [[noreturn]] static void __throw_out_of_range(const char* __msg)
    {
        __detail::__string_out_of_range(__msg);
    }

    [[noreturn]] static void __precondition_failed(const char* __msg)
    {
        __detail::__string_precondition_failed(__msg);
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
    return __XVI_STD_NS::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, const _CharT* __rhs)
{
    __lhs.append(__rhs);
    return __XVI_STD_NS::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs, basic_string<_CharT, _Traits, _Alloc>&& __rhs)
{
    auto __req_size = __lhs.size() + __rhs.size();
    if (__lhs.get_allocator() == __rhs.get_allocator() && __rhs.capacity() >= __req_size)
    {
        __rhs.insert(0, __lhs);
        return __XVI_STD_NS::move(__rhs);
    }
    
    __lhs.append(__rhs);
    return __XVI_STD_NS::move(__lhs);
}

template <class _CharT, class _Traits, class _Alloc>
constexpr basic_string<_CharT, _Traits, _Alloc>
operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, basic_string<_CharT, _Traits, _Alloc>&& __rhs)
{
    __rhs.insert(0, __lhs);
    return __XVI_STD_NS::move(__rhs);
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
    return __XVI_STD_NS::move(__rhs);
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
    return __XVI_STD_NS::move(__lhs);
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

} // namespace __XVI_STD_STRING_NS_DECL


#endif /* ifndef __SYSTEM_CXX_STRING_STRING_H */

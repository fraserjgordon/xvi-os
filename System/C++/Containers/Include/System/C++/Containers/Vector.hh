#ifndef __SYSTEM_CXX_CONTAINERS_VECTOR_H
#define __SYSTEM_CXX_CONTAINERS_VECTOR_H


#include <System/C++/Containers/Private/Config.hh>
#include <System/C++/Containers/Private/Allocator.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/MemoryResource.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/Span.hh>


namespace __XVI_STD_CONTAINERS_NS
{


//! @TODO: small vector optimisation.
//
//! @TODO: strong exception guarantee (std::move_if_noexcept).
template <class _T, class _Allocator = allocator<_T>>
class vector
{
public:

    using value_type                    = _T;
    using allocator_type                = _Allocator;
    using pointer                       = typename allocator_traits<_Allocator>::pointer;
    using const_pointer                 = typename allocator_traits<_Allocator>::const_pointer;
    using reference                     = value_type&;
    using const_reference               = const value_type&;
    using size_type                     = size_t;
    using difference_type               = ptrdiff_t;
    using iterator                      = pointer;
    using const_iterator                = const_pointer;
    using reverse_iterator              = __XVI_STD_UTILITY_NS::reverse_iterator<iterator>;
    using const_reverse_iterator        = __XVI_STD_UTILITY_NS::reverse_iterator<const_iterator>;

    constexpr vector() noexcept(noexcept(_Allocator())) :
        vector(_Allocator())
    {
    }

    constexpr explicit vector(const _Allocator& __a) noexcept :
        _M_allocator(__a)
    {
    }

    constexpr explicit vector(size_type __n, const _Allocator& __a = _Allocator()) :
        vector(__a)
    {
        auto [__ptr, __len] = __reallocate(__n);
        __default_init_elements(__ptr, __ptr + __len);
    }

    constexpr vector(size_type __n, const _T& __value, const _Allocator& __a = _Allocator()) :
        vector(__a)
    {
        auto [__ptr, __len] = __reallocate(__n);
        __copy_init_elements(__ptr, __ptr + __len, __value);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    constexpr vector(_InputIterator __first, _InputIterator __last, const _Allocator& __a = _Allocator()) :
        vector(__a)
    {
        assign(__first, __last);
    }

    constexpr vector(const vector& __x) :
        vector(__x._M_allocator)
    {
        assign(__x.begin(), __x.end());
    }

    constexpr vector(vector&& __x) noexcept :
        vector(std::move(__x._M_allocator))
    {
        using std::swap;
        swap(_M_size, __x._M_size);
        swap(_M_capacity, __x._M_capacity);
        swap(_M_elements, __x._M_elements);
    }

    constexpr vector(const vector& __x, const _Allocator& __a) :
        vector(__a)
    {
        assign(__x.begin(), __x.end());
    }

    constexpr vector(vector&& __x, const _Allocator& __a) :
        vector(__a)
    {
        using std::swap;
        swap(_M_size, __x._M_size);
        swap(_M_capacity, __x._M_capacity);
        swap(_M_elements, __x._M_elements);
    }

    constexpr vector(initializer_list<_T> __il, const _Allocator& __a = _Allocator())
        : vector(__il.begin(), __il.end(), __a)
    {
    }

    constexpr ~vector()
    {
        __deallocate();
    }

    constexpr vector& operator=(const vector& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;
        
        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
        {
            __deallocate();
            _M_allocator = __x._M_allocator;
        }

        assign(__x.begin(), __x.end());
        return *this;
    }

    constexpr vector& operator=(vector&& __x)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_move_assignment::value || allocator_traits<_Allocator>::is_always_equal::value)
    {
        // No self-assign check; move done via swaps.  
        using std::swap;     

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_size, __x._M_size);
        swap(_M_capacity, __x._M_capacity);
        swap(_M_elements, __x._M_elements);

        return *this;
    }

    constexpr vector& operator=(initializer_list<_T> __il)
    {
        assign(__il.begin(), __il.end());
        return *this;
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    constexpr void assign(_InputIterator __first, _InputIterator __last)
    {
        clear();
    
        if constexpr (__detail::__cpp17_forward_iterator<_InputIterator>)
        {
            auto __n = distance(__first, __last);
            reserve(__n);
        }

        for (; __first != __last; ++__first)
            emplace_back(*__first);
    }

    constexpr void assign(size_type __n, const _T& __value)
    {
        clear();

        for (; __n > 0; --__n)
            push_back(__value);
    }

    constexpr void assign(initializer_list<_T> __il)
    {
        assign(__il.begin(), __il.end());
    }

    constexpr allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    constexpr iterator begin() noexcept
    {
        return _M_elements;
    }

    constexpr const_iterator begin() const noexcept
    {
        return _M_elements;
    }

    constexpr iterator end() noexcept
    {
        return _M_elements + _M_size;
    }

    constexpr const_iterator end() const noexcept
    {
        return _M_elements + _M_size;
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

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr size_type size() const noexcept
    {
        return _M_size;
    }

    constexpr size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max();
    }

    constexpr size_type capacity() const noexcept
    {
        return _M_capacity;
    }

    constexpr void resize(size_type __sz)
    {
        __reallocate(__sz);
        if (_M_size < __sz)
            __default_init_elements(_M_elements + _M_size, _M_elements + __sz);
        _M_size = __sz;
    }

    constexpr void resize(size_type __sz, const _T& __fill)
    {
        __reallocate(__sz);
        if (_M_size < __sz)
            __copy_init_elements(_M_elements + _M_size, _M_elements + __sz, __fill);
        _M_size = __sz;
    }

    constexpr void reserve(size_type __sz)
    {
        try
        {
            __ensure_space(__sz);
        }
        catch (bad_alloc&)
        {
            // Ignore the exception.
        }
    }

    constexpr void shrink_to_fit()
    {
        try
        {
            __reallocate(_M_size);
        }
        catch (bad_alloc&)
        {
            // No effect if the reallocation failed.
        }
    }

    constexpr reference operator[](size_type __n)
    {
        return _M_elements[__n];
    }

    constexpr const_reference operator[](size_type __n) const
    {
        return _M_elements[__n];
    }

    constexpr const_reference at(size_type __n) const
    {
        if (__n >= _M_size)
            throw out_of_range("invalid vector index");

        return _M_elements[__n];
    }

    constexpr reference at(size_type __n)
    {
        if (__n >= _M_size)
            throw out_of_range("invalid vector index");

        return _M_elements[__n];
    }

    constexpr reference front()
    {
        return *begin();
    }

    constexpr const_reference front() const
    {
        return *begin();
    }

    constexpr reference back()
    {
        return *(end() - 1);
    }

    constexpr const_reference back() const
    {
        return *(end() - 1);
    }

    constexpr _T* data() noexcept
    {
        return _M_elements;
    }

    constexpr const _T* data() const noexcept
    {
        return _M_elements;
    }

    template <class... _Args>
    constexpr reference emplace_back(_Args&&... __args)
    {
        __grow();
        allocator_traits<_Allocator>::construct(_M_allocator, &_M_elements[_M_size], std::forward<_Args>(__args)...);
        _M_size += 1;
        return back();
    }

    constexpr void push_back(const _T& __t)
    {
        emplace_back(__t);
    }

    constexpr void push_back(_T&& __t)
    {
        emplace_back(std::move(__t));
    }

    constexpr void pop_back()
    {
        if (_M_size == 0)
            return;

        allocator_traits<_Allocator>::destroy(_M_allocator, &back());
        _M_size -= 1;
    }

    template <class... _Args>
    constexpr iterator emplace(const_iterator __position, _Args&&... __args)
    {
        auto [__where, __len] = __reallocate(_M_size + 1, __position - _M_elements, 1);
        allocator_traits<_Allocator>::construct(_M_allocator, __where, std::forward<_Args>(__args)...);
        return __where;
    }

    constexpr iterator insert(const_iterator __position, const _T& __x)
    {
        return emplace(__position, __x);
    }

    constexpr iterator insert(const_iterator __position, _T&& __x)
    {
        return emplace(__position, std::move(__x));
    }

    constexpr iterator insert(const_iterator __position, size_type __n, const _T& __x)
    {
        auto [__where, __len] = __reallocate(_M_size + __n, __position - _M_elements, __n);
        __copy_init_elements(__where, __where + __n, __x);
        return __where;
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    constexpr iterator insert(const_iterator __position, _InputIterator __first, _InputIterator __last)
    {
        if constexpr (__detail::__cpp17_forward_iterator<_InputIterator>)
        {
            auto __n = distance(__first, __last);
            auto [__where, __len] = __reallocate(_M_size + __n, __position - _M_elements, __n);
            
            for (auto __ptr = __where; __first != __last; ++__first, (void)++__ptr)
                allocator_traits<_Allocator>::construct(_M_allocator, __ptr, *__first);
            
            return __where;
        }
        else
        {
            size_type __n = 0;
            for (; __first != __last; ++__first, (void)++__n)
            {
                __position = insert(__position, *__first);
                ++__position;
            }

            return __position - (__n - 1);
        }
    }

    constexpr iterator insert(const_iterator __position, initializer_list<_T> __il)
    {
        return insert(__position, __il.begin(), __il.end());
    }

    constexpr iterator erase(const_iterator __position)
    {
        return erase(__position, __position + 1);
    }

    constexpr iterator erase(const_iterator __first, const_iterator __last)
    {
        auto __n = distance(__first, __last);
        auto __f = const_cast<iterator>(__first);
        move(__last, __last + __n, __f);
        return __f;
    }

    constexpr void swap(vector& __x)
        noexcept(allocator_traits<_Allocator>::propagate_on_container_swap::value || allocator_traits<_Allocator>::is_always_equal::value)
    {
        using std::swap;
        
        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_size, __x._M_size);
        swap(_M_capacity, __x._M_capacity);
        swap(_M_elements, __x._M_elements);
    }

    constexpr void clear() noexcept
    {
        __truncate_in_place(0);
    }

private:

    [[no_unique_address]] _Allocator _M_allocator = {};

    size_type   _M_size         = 0;
    size_type   _M_capacity     = 0;

    pointer     _M_elements     = nullptr;


    static constexpr size_type __npos = numeric_limits<size_type>::max();

    // Resizing parameters. These should keep overhead at ~25%.
    static constexpr size_type __resize_numerator   = 4;
    static constexpr size_type __resize_denominator = 3;


    constexpr void __truncate_in_place(size_type __new_length) noexcept
    {
        // Destroy all elements past the truncation point.
        for (size_type __i = __new_length; __i < _M_size; ++__i)
            allocator_traits<_Allocator>::destroy(_M_allocator, &_M_elements[__i]);
        _M_size = __new_length;
    }

    constexpr void __default_init_elements(pointer __first, pointer __last)
    {
        for (; __first < __last; ++__first)
            allocator_traits<_Allocator>::construct(_M_allocator, __first);
    }

    constexpr void __copy_init_elements(pointer __first, pointer __last, const _T& __value)
    {
        for (; __first < __last; ++__first)
            allocator_traits<_Allocator>::construct(_M_allocator, __first, __value);
    }

    constexpr void __deallocate()
    {
        __truncate_in_place(0);
        
        if (_M_elements)
            allocator_traits<_Allocator>::deallocate(_M_allocator, _M_elements, _M_capacity);
        _M_elements = nullptr;
        _M_size = _M_capacity = 0;
    }

    constexpr void __ensure_space(size_type __n)
    {
        if (__n <= _M_capacity)
            return;

        __reallocate(__n);
    }

    constexpr void __grow()
    {
        auto __min_size = _M_size + 1;
        if (__min_size < _M_capacity)
            return;

        // New size = ceil(size / denominator) * numerator.
        auto __size = (__min_size + __resize_denominator - 1) / __resize_denominator;
        __size *= __resize_numerator;

        // Sanity check.
        if (__size < __min_size) [[unlikely]]
            __size = max_size();
        
        __reallocate(__size);
    }

    constexpr span<_T> __ensure_space_and_create_gap(size_type __n, size_type __gap_pos, size_type __gap_len)
    {
        if (_M_capacity <= __n)
        {
            auto [__ptr, __len] = __reallocate(__n, __gap_pos, __gap_len);
            return {__ptr, __len};
        }

        // Shift all of the elements along.
        for (size_type __i = __gap_pos; __i < _M_size; ++__i)
        {
            auto __from = _M_size - __i - 1;
            auto __to = __from + __gap_len;
            allocator_traits<_Allocator>::construct(_M_allocator, &_M_elements[__to], std::move(_M_elements[__from]));
            allocator_traits<_Allocator>::destroy(_M_allocator, &_M_elements[__from]);
        }

        _M_size += __gap_len;

        return {&_M_elements[__gap_pos], __gap_len};
    }

    constexpr pair<pointer, size_type> __reallocate(size_type __req_capacity, size_type __gap_pos = __npos, size_type __gap_len = 0)
    {
        // Check for overflow.
        if (__req_capacity > max_size() || __gap_len > __req_capacity)
            throw length_error("invalid length for std::vector");

        // Check for truncation.
        if (__req_capacity < _M_size + __gap_len)
            __truncate_in_place(__req_capacity - __gap_len);

        // If the gap is at the end, we don't need to explicitly create it - just leave the space uninitialised.
        if (__gap_pos == __npos || __gap_pos >= _M_size)
            __gap_pos = 0;

        // Allocate the new memory. This may throw.
        pointer __p = allocator_traits<_Allocator>::allocate(_M_allocator, __req_capacity);
        if (!__p)
            throw bad_alloc();

        auto __current = _M_elements;
        auto __current_size = _M_size;
        auto __current_capacity = _M_capacity;

        if (__current)
        {
            try
            {
                // Copy all data up until the position of the gap.
                for (size_type __i = 0; __i < __gap_pos; ++__i)
                {
                    allocator_traits<_Allocator>::construct(_M_allocator, &__p[__i], std::move(__current[__i]));
                    allocator_traits<_Allocator>::destroy(_M_allocator, &__current[__i]);
                }

                // Copy the remainder of the data to after the gap.
                for (size_type __i = __gap_pos; __i < __current_size; ++__i)
                {
                    allocator_traits<_Allocator>::construct(_M_allocator, &__p[__i + __gap_len], std::move(__current[__i]));
                    allocator_traits<_Allocator>::destroy(_M_allocator, &__current[__i]);
                }
            }
            catch (...)
            {
                // Deallocate the new memory.
                allocator_traits<_Allocator>::deallocate(_M_allocator, __p, __req_capacity);
                throw;
            }
        }

        // Elements moved successfully.
        _M_size = __current_size + __gap_len;
        _M_capacity = __req_capacity;
        _M_elements = __p;

        // Free the old memory.
        if (__current)
            allocator_traits<_Allocator>::deallocate(_M_allocator, __current, __current_capacity);

        // Return a span representing the gap.
        return {&_M_elements[__gap_pos], __gap_len};
    }
};

template <class _InputIterator, class _Allocator = allocator<__detail::__iter_value_t<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator>
          && __detail::__is_allocator<_Allocator>
vector(_InputIterator, _InputIterator, _Allocator = _Allocator()) -> vector<__detail::__iter_value_t<_InputIterator>, _Allocator>;

template <class _T, class _Allocator>
constexpr bool operator==(const vector<_T, _Allocator>& __x, const vector<_T, _Allocator>& __y)
{
    if (&__x == &__y)
        return true;

    if (__x.size() != __y.size())
        return false;

    return equal(__x.begin(), __x.end(), __y.begin());
}

template <class _T, class _Allocator>
constexpr __detail::__synth_three_way_result<_T> operator<=>(const vector<_T, _Allocator>& __x, const vector<_T, _Allocator>& __y)
{
    return lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}

template <class _T, class _Allocator>
constexpr void swap(vector<_T, _Allocator>& __x, vector<_T, _Allocator>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Allocator, class _U>
constexpr typename vector<_T, _Allocator>::size_type erase(vector<_T, _Allocator>& __c, const _U& __value)
{
    auto __it = remove(__c.begin(), __c.end(), __value);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());
    return __r;
}

template <class _T, class _Allocator, class _Predicate>
constexpr typename vector<_T, _Allocator>::size_type erase_if(vector<_T, _Allocator>& __c, _Predicate __predicate)
{
    auto __it = remove(__c.begin(), __c.end(), __predicate);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());
    return __r;
}


template <class _Allocator>
class vector<bool, _Allocator>;


template <class _T> struct hash;
template <class _Allocator>
struct hash<vector<bool, _Allocator>>;


namespace pmr
{


template <class _T>
using vector = __XVI_STD_CONTAINERS_NS::vector<_T, pmr::polymorphic_allocator<_T>>;


} // namespace pmr


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_VECTOR_H */

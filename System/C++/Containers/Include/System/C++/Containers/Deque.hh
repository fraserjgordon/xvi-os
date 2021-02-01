#ifndef __SYSTEM_CXX_CONTAINERS_DEQUE_H
#define __SYSTEM_CXX_CONTAINERS_DEQUE_H


#include <System/C++/Containers/Private/Config.hh>
#include <System/C++/Containers/Private/Allocator.hh>
#include <System/C++/Containers/Vector.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/MemoryResource.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace __XVI_STD_CONTAINERS_NS
{


template <class _T, class _Allocator = __XVI_STD_UTILITY_NS::allocator<_T>>
class deque
{
public:

    template <bool _Const> class __iterator;
    struct __array;
    
    using value_type                = _T;
    using allocator_type            = _Allocator;
    using pointer                   = typename allocator_traits<_Allocator>::pointer;
    using const_pointer             = typename allocator_traits<_Allocator>::const_pointer;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = __iterator<false>;
    using const_iterator            = __iterator<true>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;


    template <bool _Const>
    class __iterator
    {
    public:

        using value_type        = std::conditional_t<_Const, const _T, _T>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr __iterator() = default;
        constexpr __iterator(const __iterator&) = default;
        constexpr __iterator(__iterator&&) = default;

        constexpr __iterator(const __iterator<!_Const>& __x)
            requires _Const :
            _M_deque(__x._M_deque), _M_offset(__x._M_offset)
        {
        }

        constexpr __iterator& operator=(const __iterator&) = default;
        constexpr __iterator& operator=(__iterator&&) = default;

        constexpr __iterator& operator=(const __iterator<!_Const>& __x)
            requires _Const
        {
            _M_deque = __x._M_deque;
            _M_offset = __x._M_offset;
            return *this;
        }

        constexpr ~__iterator() = default;

        constexpr reference operator*() const
        {
            return _M_deque->operator[](_M_offset);
        }

        constexpr pointer operator->() const
        {
            return &_M_deque->operator[](_M_offset);
        }

        constexpr __iterator& operator++()
        {
            ++_M_offset;
            return *this;
        }

        constexpr __iterator& operator--()
        {
            --_M_offset;
            return *this;
        }

        constexpr __iterator operator++(int)
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator operator--(int)
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __d)
        {
            _M_offset += __d;
            return *this;
        }

        constexpr __iterator& operator-=(difference_type __d)
        {
            _M_offset -= __d;
            return *this;
        }

        constexpr __iterator operator-(difference_type __n) const
        {
            auto __tmp = *this;
            __tmp -= __n;
            return __tmp;
        }

        constexpr difference_type operator-(const __iterator& __x) const
        {
            return static_cast<difference_type>(_M_offset - __x._M_offset);
        }

        constexpr reference operator[](difference_type __d) const
        {
            return _M_deque->operator[](_M_offset + __d);
        }

        friend constexpr __iterator operator+(const __iterator& __x, difference_type __d)
        {
            auto __tmp = __x;
            __tmp += __d;
            return __tmp;
        }

        friend constexpr __iterator operator+(difference_type __d, const __iterator& __x)
        {
            return operator+(__x, __d);
        }

        constexpr bool operator==(const __iterator& __x) const = default;
        constexpr auto operator<=>(const __iterator& __x) const = default;

    private:

        friend class __iterator<!_Const>;
        friend class deque;

        using __deque_ptr = conditional_t<_Const, const deque*, deque*>;

        __deque_ptr _M_deque    = nullptr;
        std::size_t _M_offset   = 0;
        
        constexpr __iterator(__deque_ptr __d, std::size_t __offset) :
            _M_deque(__d), _M_offset(__offset)
        {
        }
    };
    
    struct __array
    {
        using __elem_t = aligned_storage_t<sizeof(_T), alignof(_T)>;

        static constexpr auto __min_element_count   = 16;
        static constexpr auto __min_array_size      = 1024;

        static constexpr auto __array_size          = (__min_element_count * sizeof(_T) < __min_array_size)
                                                    ? (__min_array_size / sizeof(_T))
                                                    : __min_element_count;

        __elem_t _M_elems[__array_size];
    };


    deque() :
        deque(_Allocator())
    {
    }

    explicit deque(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    explicit deque(size_type __n, const _Allocator& __a = _Allocator()) :
        deque(__a)
    {
        resize(__n);
    }

    deque(size_type __n, const _T& __value, const _Allocator& __a = _Allocator()) :
        deque(__a)
    {
        assign(__n, __value);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    deque(_InputIterator __first, _InputIterator __last, const _Allocator& __a = _Allocator()) :
        deque(__a)
    {
        assign(__first, __last);
    }

    deque(const deque& __x) :
        deque(__x._M_allocator)
    {
        assign(__x.begin(), __x.end());
    }

    deque(deque&& __x) :
        deque()
    {
        using std::swap;

        swap(_M_storage, __x._M_storage);
        swap(_M_offset, __x._M_offset);
        swap(_M_size, __x._M_size);
    }

    deque(const deque& __x, const _Allocator& __a) :
        deque(__a)
    {
        assign(__x.begin(), __x.end());
    }

    deque(deque&& __x, const _Allocator& __a) :
        deque(__a)
    {
        using std::swap;

        swap(_M_storage, __x._M_storage);
        swap(_M_offset, __x._M_offset);
        swap(_M_size, __x._M_size);
    }

    deque(initializer_list<_T> __il, const _Allocator& __a = _Allocator()) :
        deque(__il.begin(), __il.end(), __a)
    {
    }

    ~deque()
    {
        clear();
    }

    deque& operator=(const deque& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
        {
            __free_all();
            _M_allocator = __x._M_allocator;
        }

        assign(__x.begin(), __x.end());
        return *this;
    }

    deque& operator=(deque&& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        // No self-assign check; move done via swaps.
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_storage, __x._M_storage);
        swap(_M_offset, __x._M_offset);
        swap(_M_size, __x._M_size);

        return *this;
    }

    deque& operator=(initializer_list<_T> __il)
    {
        assign(__il);
        return *this;
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    void assign(_InputIterator __first, _InputIterator __last)
    {
        clear();

        if constexpr (__detail::__cpp17_forward_iterator<_InputIterator>)
        {
            auto __n = distance(__first, __last);
            __ensure_elems_back(__n);
        }

        for (; __first != __last; ++__first)
            emplace_back(*__first);
    }

    void assign(size_type __n, const _T& __x)
    {
        clear();

        for (; __n > 0; --__n)
            push_back(__x);
    }

    void assign(initializer_list<_T> __il)
    {
        assign(__il.begin(), __il.end());
    }

    allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    iterator begin() noexcept
    {
        return iterator(this, 0);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(this, 0);
    }

    iterator end() noexcept
    {
        return iterator(this, _M_size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(this, _M_size);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type size() const noexcept
    {
        return _M_size;
    }

    size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max() / sizeof(value_type);
    }

    void resize(size_type __sz)
    {
        __reallocate(__sz);
        if (_M_size < __sz)
            __default_construct_elem_range(_M_size, __sz - _M_size);
        _M_size = __sz;
    }

    void resize(size_type __sz, const _T& __x)
    {
        __reallocate(__sz);
        if (_M_size < __sz)
            __copy_construct_elem_range(_M_size, __sz - _M_size, __x);
        _M_size = __sz;
    }

    void shrink_to_fit()
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

    reference operator[](size_type __n)
    {
        return *__get_elem(__n);
    }

    const_reference operator[](size_type __n) const
    {
        return *__get_elem(__n);
    }

    reference at(size_type __n)
    {
        if (__n >= _M_size)
            throw out_of_range("invalid deque index");

        return operator[](__n);
    }

    const_reference at(size_type __n) const
    {
        if (__n >= _M_size)
            throw out_of_range("invalid deque index");

        return operator[](__n);
    }

    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    reference back()
    {
        return *--end();
    }

    const_reference back() const
    {
        return *--end();
    }

    template <class... _Args>
    reference emplace_front(_Args&&... __args)
    {
        return *emplace(begin(), std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    reference emplace_back(_Args&&... __args)
    {
        return *emplace(end(), std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    iterator emplace(const_iterator __position, _Args&&... __args)
    {
        _Allocator __a = _M_allocator;
        auto __where = __position._M_offset;
        __make_gap(__a, __where, 1);

        allocator_traits<_Allocator>::construct(__a, __get_elem(__where), std::forward<_Args>(__args)...);

        return iterator(this, __where);
    }

    void push_front(const _T& __x)
    {
        emplace_front(__x);
    }

    void push_front(_T&& __x)
    {
        emplace_front(std::move(__x));
    }

    void push_back(const _T& __x)
    {
        emplace_back(__x);
    }

    void push_back(_T&& __x)
    {
        emplace_back(std::move(__x));
    }

    iterator insert(const_iterator __position, const _T& __x)
    {
        return emplace(__position, __x);
    }

    iterator insert(const_iterator __position, _T&& __x)
    {
        return emplace(__position, std::move(__x));
    }

    iterator insert(const_iterator __position, size_type __n, const _T& __x)
    {
        _Allocator __a = _M_allocator;
        auto __off = __position._M_offset;
        
        __make_gap(__a, __off, __n);
        __copy_construct_elem_range(__off, __n, __x);

        return iterator(this, __off);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    iterator insert(const_iterator __position, _InputIterator __first, _InputIterator __last)
    {
        if constexpr (__detail::__cpp17_forward_iterator<_InputIterator>)
        {
            _Allocator __a = _M_allocator;            

            auto __where = __position._M_offset;
            auto __n = distance(__first, __last);
            __make_gap(__a, __where, __n);

            for (auto __idx = __where; __first != __last; ++__first, (void)++__idx)
            {
                auto [__arr, __off] = __index(__idx);
                auto* __p = reinterpret_cast<_T*>(&_M_storage[__arr]->_M_elems[__off]);

                allocator_traits<_Allocator>::construct(__a, __p, *__first);
            }

            return iterator(this, __where);
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

    iterator insert(const_iterator __position, initializer_list<_T> __il)
    {
        return insert(__position, __il.begin(), __il.end());
    }

    void pop_front()
    {
        if (_M_size == 0)
            return;

        erase(begin());
    }

    void pop_back()
    {
        if (_M_size == 0)
            return;

        erase(end() - 1);
    }

    iterator erase(const_iterator __position)
    {
        return erase(__position, __position + 1);
    }

    iterator erase(const_iterator __first, const_iterator __last)
    {
        auto __where = __first._M_offset;
        __erase(__where, __last._M_offset);
        return iterator(this, __where);
    }

    void swap(deque& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_storage, __x._M_storage);
        swap(_M_offset, __x._M_offset);
        swap(_M_size, __x._M_size);
    }

    void clear()
    {
        __free_all();
    }


private:

    using __array_alloc = typename allocator_traits<_Allocator>::template rebind_alloc<__array>;


    [[no_unique_address]] __array_alloc _M_allocator = {};

    vector<__array*> _M_storage = {};

    size_type _M_offset = 0;
    size_type _M_size = 0;


    __array* __get_array(size_type __i) const
    {
        auto [__idx, __off] = __index(__i);
        if (__idx >= _M_storage.size())
            return nullptr;

        return _M_storage[__idx];
    }

    _T* __get_elem(size_type __i) const
    {
        auto [__idx, __off] = __index(__i);
        if (__idx >= _M_storage.size())
            return nullptr;

        return reinterpret_cast<_T*>(&_M_storage[__idx]->_M_elems[__i]);
    }

    size_type __total_capacity() const
    {
        return _M_storage.size() * __array::__array_size;
    }

    size_type __front_capacity() const
    {
        return _M_offset;
    }

    size_type __back_capacity() const
    {
        return __total_capacity() - __front_capacity();
    }

    size_type __array_expand_factor(size_type __requested)
    {
        static constexpr auto __expand_num  = 3;
        static constexpr auto __expand_den  = 2;
        
        auto __curr = _M_size / __array::__array_size;
        auto __target = (__curr * __expand_num) / __expand_den;

        return (__target > __requested) ? __target : __requested;   
    }

    void __insert_arrays_front(size_type __count)
    {
        __count = __array_expand_factor(__count);
        _M_storage.insert(_M_storage.begin(), __count, nullptr);
        _M_offset += (__count * __array::__array_size);
    }

    void __insert_arrays_back(size_type __count)
    {
        __count = __array_expand_factor(__count);
        _M_storage.insert(_M_storage.end(), __count, nullptr);
    }

    void __ensure_elems_front(size_type __count)
    {
        auto __curr = __front_capacity();
        if (__curr < __count)
        {
            auto __extra = __count - __curr;
            auto __narrays = (__extra + __array::__array_size - 1) / __array::__array_size;

            __insert_arrays_front(__narrays);
        }

        auto [__arr, __off] = __index(0);
        for (; __count >= __array::__array_size; __count -= __array::__array_size, --__arr)
        {
            if (!_M_storage[__arr])
                _M_storage[__arr] = __alloc_array();
        }
    }

    void __reallocate(size_type __sz)
    {
        __ensure_elems_front(__sz);
    }

    void __ensure_elems_back(size_type __count)
    {
        auto __curr = __back_capacity();
        if (__curr < __count)
        {
            auto __extra = __count - __curr;
            auto __narrays = (__extra + __array::__array_size - 1) / __array::__array_size;

            __insert_arrays_back(__narrays);
        }

        auto [__arr, __off] = __index(_M_size);
        for (; __count >= __array::__array_size; __count -= __array::__array_size, ++__arr)
        {
            if (!_M_storage[__arr])
                _M_storage[__arr] = __alloc_array();
        }
    }

    void __clean_front_arrays()
    {
        auto [__arr, __off] = __index(0);

        for (; __arr > 0; --__arr)
        {
            auto __i = __arr - 1;
            if (_M_storage[__i])
            {
                __dealloc_array(_M_storage[__i]);
                _M_storage[__i] = nullptr;
            }
        }
    }

    void __clean_back_arrays()
    {
        auto [__arr, __off] = __index(_M_size + __array::__array_size - 1);

        for (auto __end = _M_storage.size(); __arr < __end; ++__arr)
        {
            if (_M_storage[__arr])
            {
                __dealloc_array(_M_storage[__arr]);
                _M_storage[__arr] = nullptr;
            }
        }
    }

    void __trim_front_arrays()
    {
        __clean_front_arrays();
        auto [__arr, __off] = __index(0);

        for (size_type __i = 0; __i < __arr; ++__i)
        {
            __dealloc_array(_M_storage[__i]);
            _M_storage[__i] = nullptr;
        }

        _M_storage.erase(_M_storage.begin(), _M_storage.begin() + __arr);
    }

    void __trim_back_arrays()
    {
        __clean_back_arrays();
        auto [__arr, __off] = __index(_M_size + __array::__array_size - 1);

        for (size_type __i = 0; __i < __arr; ++__i)
        {
            __dealloc_array(_M_storage[__i]);
            _M_storage[__i] = nullptr;
        }

        _M_storage.erase(_M_storage.begin() + __arr, _M_storage.end());
    }

    void __trim_arrays()
    {
        __trim_back_arrays();
        __trim_front_arrays();

        _M_storage.shrink_to_fit();
    }

    void __make_gap(_Allocator& __a, size_type __pos, size_type __len)
    {
        if (__pos == 0)
        {
            __ensure_elems_front(__len);
            _M_offset -= __len;
            _M_size += __len;
            return;
        }

        if (__pos == _M_size)
        {
            __ensure_elems_back(__len);
            _M_size += __len;
            return;
        }

        auto __half = _M_size / 2;
        if (__pos >= __half)
        {
            __ensure_elems_back(__len);
            auto __count = _M_size - __pos;
            auto __from = _M_size - __count;
            auto __to = __pos + __len;
            
            __move_or_copy_construct_elem_range(__a, __from, __to, __count);
        }
        else
        {
            __ensure_elems_front(__len);
            auto __count = __pos;
            auto __from = _M_offset;
            auto __to = _M_offset - __len;
            
            __move_or_copy_construct_elem_range(__a, __from, __to, __count);

            _M_offset -= __len;
        }

        _M_size += __len;
        __destroy_elem_range(__a, __pos, __len);
    }

    void __remove_elem_range(_Allocator& __a, size_type __pos, size_type __len)
    {
        if (__pos == 0)
        {
            _M_offset += __len;
            _M_size -= __len;

            __destroy_elem_range(__a, _M_offset - __len, __len);

            return;
        }

        if (__pos == _M_size)
        {
            _M_size -= __len;

            __destroy_elem_range(__a, _M_size, __len);

            return;
        }

        auto __half = _M_size / 2;
        if (__pos >= __half)
        {
            auto __count = _M_size - (__pos + __len);
            auto __from = _M_size - __count;
            auto __to = __pos;

            __move_assign_elem_range(__from, __to, __count);

            _M_size -= __len;

            __destroy_elem_range(__a, _M_size, __len);
        }
        else
        {
            auto __count = __pos;
            auto __from = _M_offset;
            auto __to = _M_offset + __len;

            __move_assign_elem_range(__from, __to, __count);

            _M_offset += __len;
            _M_size -= __len;

            __destroy_elem_range(__a, _M_offset - __len, __len);
        }
    }

    void __erase(size_type __first, size_type __last)
    {
        _Allocator __a = _M_allocator;
        __remove_elem_range(__a, __first, __last);
    }

    void __free_all()
    {
        __erase(0, _M_size);
        __trim_arrays();
    }

    void __move_or_copy_construct_elem(_Allocator& __a, size_type __from, size_type __to)
        noexcept(is_nothrow_move_constructible_v<_T>)
    {
        auto [__farr, __foff] = __index(__from);
        auto [__tarr, __toff] = __index(__to);

        auto* __f = reinterpret_cast<_T*>(&_M_storage[__farr]->_M_elems[__foff]);
        auto* __t = reinterpret_cast<_T*>(&_M_storage[__tarr]->_M_elems[__toff]);

        allocator_traits<_Allocator>::construct(__a, __t, std::move_if_noexcept(*__f));
    }

    void __move_assign_elem(size_type __from, size_type __to)
    {
        auto [__farr, __foff] = __index(__from);
        auto [__tarr, __toff] = __index(__to);

        auto* __f = reinterpret_cast<_T*>(&_M_storage[__farr]->_M_elems[__foff]);
        auto* __t = reinterpret_cast<_T*>(&_M_storage[__tarr]->_M_elems[__tarr]);

        *__t = std::move(*__f);
    }

    void __move_or_copy_construct_elem_range(_Allocator& __a, size_type __from, size_type __to, size_type __len)
    {
        if (__from > __to)
        {
            auto __l = __len;
            try
            {
                for (; __l > 0; --__l, ++__from, ++__to)
                    __move_or_copy_construct_elem(__a, __from, __to);
            }
            catch (...)
            {
                auto __elems = __len - __l;
                __destroy_elem_range(__a, __to - __elems, __elems);
                
                throw;
            }
        }
        else if (__from < __to)
        {
            auto __l = __len;
            try
            {
                __from += __len - 1;
                __to += __len - 1;
                for (; __l > 0; --__l, --__from, --__to)
                    __move_or_copy_construct_elem(__a, __from, __to);
            }
            catch (...)
            {
                auto __elems = __len - __l;
                __destroy_elem_range(__a, __to + 1, __elems);

                throw;
            }
        }
    }

    void __move_assign_elem_range(size_type __from, size_type __to, size_type __len)
    {
        if (__from > __to)
        {
            for (; __len > 0; --__len, ++__from, ++__to)
                __move_assign_elem(__from, __to);
        }
        else if (__from < __to)
        {
            __from += __len - 1;
            __to += __len - 1;
            for (; __len > 0; --__len, --__from, --__to)
                __move_assign_elem(__from, __to);
        }
    }

    void __default_construct_elem_range(size_type __where, size_type __len)
    {
        _Allocator __a = _M_allocator;
        
        for (; __len > 0; --__len, ++__where)
        {
            auto [__arr, __off] = __index(__where);
            auto* __p = reinterpret_cast<_T*>(&_M_storage[__arr]->_M_elems[__off]);

            allocator_traits<_Allocator>::construct(__a, __p);
        }
    }

    void __copy_construct_elem_range(size_type __where, size_type __len, const value_type& __x)
    {
        _Allocator __a = _M_allocator;
        
        __copy_construct_elem_range(__a, __where, __len, __x);
    }

    void __copy_construct_elem_range(_Allocator& __a, size_type __where, size_type __len, const value_type& __x)
    {
        for (; __len > 0; --__len, ++__where)
        {
            auto [__arr, __off] = __index(__where);
            auto* __p = reinterpret_cast<_T*>(&_M_storage[__arr]->_M_elems[__off]);

            allocator_traits<_Allocator>::construct(__a, __p, __x);
        }
    }

    void __destroy_elem_range(_Allocator& __a, size_type __where, size_type __len)
    {
        for (; __len > 0; --__len, ++__where)
        {
            auto [__arr, __off] = __index(__where);
            auto* __p = reinterpret_cast<_T*>(&_M_storage[__arr]->_M_elems[__off]);

            allocator_traits<_Allocator>::destroy(__a, __p);
        }
    }

    __array* __alloc_array()
    {
        auto* __p = allocator_traits<__array_alloc>::allocate(_M_allocator, 1);
        allocator_traits<__array_alloc>::construct(_M_allocator, __p);

        return __p;
    }

    void __dealloc_array(__array* __p)
    {
        allocator_traits<__array_alloc>::destroy(_M_allocator, __p);
        allocator_traits<__array_alloc>::deallocate(_M_allocator, __p, 1);
    }

    pair<size_type, size_type> __index(size_type __i) const
    {
        __i += _M_offset;
        return {__i / __array::__array_size, __i % __array::__array_size};
    }
};

template <class _InputIterator, class _Allocator = allocator<__detail::__iter_value_t<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
deque(_InputIterator, _InputIterator, _Allocator = _Allocator()) -> deque<_InputIterator, _Allocator>;


template <class _T, class _Allocator>
bool operator==(const deque<_T, _Allocator>& __x, const deque<_T, _Allocator>& __y);

template <class _T, class _Allocator>
__detail::__synth_three_way_result<_T> operator<=>(const deque<_T, _Allocator>& __x, const deque<_T, _Allocator>& __y);

template <class _T, class _Allocator>
void swap(deque<_T, _Allocator>& __x, deque<_T, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Allocator, class _U>
typename deque<_T, _Allocator>::size_type erase(deque<_T, _Allocator>& __c, const _U& __value)
{
    auto __it = remove(__c.begin(), __c.end(), __value);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());

    return __r;
}

template <class _T, class _Allocator, class _Pred>
typename deque<_T, _Allocator>::size_type erase_if(deque<_T, _Allocator>& __c, _Pred __pred)
{
    auto __it = remove_if(__c.begin(), __c.end(), __pred);
    auto __r = distance(__it, __c.end());
    __c.erase(__it, __c.end());

    return __r;
}


namespace pmr
{

template <class _T>
using deque = __XVI_STD_CONTAINERS_NS::deque<_T, pmr::polymorphic_allocator<_T>>;

} // namespace pmr


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_DEQUE_H */

#ifndef __SYSTEM_CXX_CONTAINERS_LIST_H
#define __SYSTEM_CXX_CONTAINERS_LIST_H


#include <System/C++/Containers/Private/Config.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/MemoryResource.hh>


namespace __XVI_STD_CONTAINERS_NS
{


template <class _T, class _Allocator = allocator<_T>>
class list
{
public:

    template <bool _Const> class __iterator;
    struct __element;

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

    struct __element
    {
        __element*      __next  = nullptr;
        __element*      __prev  = nullptr;
        value_type      __data;
    };

    template <bool _Const>
    class __iterator
    {
    public:

        friend class list;

        using value_type        = _T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = _T*;
        using reference         = _T&;
        using iterator_category = std::bidirectional_iterator_tag;

        __iterator() = default;
        __iterator(const __iterator&) = default;

        __iterator(const __iterator<!_Const> __i)
            requires (_Const) :
            _M_element(__i._M_element)
        {
        }

        __iterator& operator=(const __iterator&) = default;

        __iterator& operator=(const __iterator<!_Const> __i)
            requires (_Const)
        {
            _M_element = __i._M_element;
            return *this;
        }

        ~__iterator() = default;

        bool operator==(const __iterator& __x) const = default;

        reference operator*() const
            requires (!_Const)
        {
            return _M_element->__data;
        }

        const_reference operator*() const
            requires (_Const)
        {
            return _M_element->__data;
        }

        pointer operator->() const
            requires (!_Const)
        {
            return &_M_element->__data;
        }

        const_pointer operator->() const
            requires (_Const)
        {
            return &_M_element->__data;
        }

        __iterator& operator++()
        {
            _M_element = _M_element->__next;
            return *this;
        }

        __iterator operator++(int)
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        __iterator& operator--()
        {
            if (_M_element)
                _M_element = _M_element->__prev;
            else
                _M_element = _M_parent->_M_last;

            return *this;
        }

        __iterator operator--(int)
        {
            auto __tmp = *this;
            --__tmp;
            return __tmp;
        }

    private:

        //! @TODO: keeping a parent pointer causes problems re: iterator invalidation when lists are merged/spliced.
        const list*     _M_parent  = nullptr;
        __element*      _M_element = nullptr;

        explicit __iterator(const list* __parent, __element* __e) :
            _M_parent(__parent), _M_element(__e)
        {
        }

        __iterator<!_Const> __convert() const
            requires (_Const)
        {
            return __iterator<!_Const>(_M_parent, _M_element);
        }
    };


    list() :
        list(_Allocator())
    {
    }

    explicit list(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    explicit list(size_type __n, const _Allocator& __a) :
        list(__a)
    {
        resize(__n);
    }

    list(size_type __n, const _T& __x, const _Allocator& __a = _Allocator()) :
        list(__a)
    {
        assign(__n, __x);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    list(_InputIterator __first, _InputIterator __last, const _Allocator& __a = _Allocator()) :
        list(__a)
    {
        assign(__first, __last);
    }

    list(const list& __x) :
        list(__x._M_allocator)
    {
        assign(__x.begin(), __x.end());
    }

    list(list&& __x) :
        _M_allocator(std::move(__x._M_allocator))
    {
        _M_list = __x._M_list;
        _M_last = __x._M_last;
        _M_count = __x._M_count;

        __x._M_list = nullptr;
        __x._M_last = nullptr;
        __x._M_count = 0;
    }

    list(const list& __x, const _Allocator& __a) :
        list(__a)
    {
        assign(__x.begin(), __x.end());
    }

    list(list&& __x, const _Allocator& __a) :
        list(__a)
    {
        _M_list = __x._M_list;
        _M_last = __x._M_last;
        _M_count = __x._M_count;

        __x._M_list = nullptr;
        __x._M_last = nullptr;
        __x._M_count = 0;
    }

    list(initializer_list<_T>& __il, const _Allocator& __a = _Allocator()) :
        list(__a)
    {
        assign(__il);
    }

    ~list()
    {
        clear();
    }

    list& operator=(const list& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;
        
        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
        {
            clear();
            _M_allocator = __x._M_allocator;
        }

        assign(__x.begin(), __x.end());
        return *this;
    }

    list& operator=(list&& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        // No self-assign check; move done via swaps.
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_list, __x._M_list);
        swap(_M_last, __x._M_last);
        swap(_M_count, __x._M_count);

        return *this;
    }

    list& operator=(initializer_list<_T> __il)
    {
        assign(__il.begin(), __il.end());
        return *this;
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    void assign(_InputIterator __first, _InputIterator __last)
    {
        // Overwrite existing elements as much as possible.
        auto __i = begin();
        auto __end = end();
        for (; __i != __end && __first != __last; ++__i, ++__first)
            *__i = *__first;

        // If we have elements left over, delete them.
        if (__i != __end)
        {
            erase(__i, __end);
            return;
        }

        // Insert the remaining elements.
        insert(__i, __first, __last);
    }

    void assign(size_type __n, const _T& __x)
    {
        // Overwrite existing elements as much as possible.
        auto __i = begin();
        auto __end = end();
        for (; __i != __end && __n > 0; ++__i, --__n)
            *__i = __x;

        // If we have elements left over, delete them.
        if (__i != __end)
        {
            erase(__i, __end);
            return;
        }

        // Insert the remaining elements.
        insert(__i, __n, __x);
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
        return iterator{this, _M_list};
    }

    const_iterator begin() const noexcept
    {
        return const_iterator{this, _M_list};
    }

    iterator end() noexcept
    {
        return iterator{this, nullptr};
    }

    const_iterator end() const noexcept
    {
        return const_iterator{this, nullptr};
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
        return _M_count != 0;
    }

    size_type size() const noexcept
    {
        return _M_count;
    }

    size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max() / sizeof(__element);
    }

    void resize(size_type __sz)
    {
        if (__sz > size())
            for(__sz -= size(); __sz > 0; --__sz)
                emplace_back();
        else
        {
            auto __i = begin();
            advance(__i, size() - __sz);
            erase(__i, end());
        }
    }

    void resize(size_type __sz, const _T& __x)
    {
        if (__sz > size())
            insert(end(), __sz - size(), __x);
        else
        {
            auto __i = begin();
            advance(__i, size() - __sz);
            erase(__i, end());
        }
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
        return _M_last->__data;
    }

    const_reference back() const
    {
        return _M_last->__data;
    }
    
    template <class... _Args>
    reference emplace_front(_Args&&... __args)
    {
        auto __elem = __insert_new_element_after(nullptr, std::forward<_Args>(__args)...);
        return __elem->__data;
    }

    template <class... _Args>
    reference emplace_back(_Args&&... __args)
    {
        auto __elem = __insert_new_element_after(_M_last, std::forward<_Args>(__args)...);
        return __elem->__data;
    }

    void push_front(const _T& __x)
    {
        emplace_front(__x);
    }

    void push_front(_T&& __x)
    {
        emplace_front(std::move(__x));
    }

    void pop_front()
    {
        if (_M_list)
            __remove_and_delete_element(_M_list);
    }

    void push_back(const _T& __x)
    {
        emplace_back(__x);
    }

    void push_back(_T&& __x)
    {
        emplace_back(std::move(__x));
    }

    void pop_back()
    {
        if (_M_last)
            __remove_and_delete_element(_M_last);
    }

    template <class... _Args>
    iterator emplace(const_iterator __position, _Args&&... __args)
    {
        auto* __elem = __insert_new_element_before(__position._M_element, std::forward<_Args>(__args)...);
        return iterator{this, __elem};
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
        for (; __n > 0; --__n)
            __position = insert(__position, __x);

        return iterator{this, __position._M_element};
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    iterator insert(const_iterator __position, _InputIterator __first, _InputIterator __last)
    {
        auto __prev = (__position._M_element) ? __position._M_element->__prev : nullptr;

        for (; __first != __last; ++__first)
            emplace(__position, *__first);

        return (__prev) ? iterator{this, __prev} : begin();
    }

    iterator insert(const_iterator __position, initializer_list<_T> __il)
    {
        return insert(__position, __il.begin(), __il.end());
    }

    iterator erase(const_iterator __position)
    {
        bool __last = (__position._M_element == _M_last);
        auto __next = (__last) ? nullptr : __position._M_element->__next;
        
        __remove_and_delete_element(__position._M_element);

        return (__last) ? end() : iterator{this, __next};
    }

    iterator erase(const_iterator __first, const_iterator __last)
    {
        while (__first != __last)
            __first = erase(__first);

        return __first.__convert();
    }

    void swap(list& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        using std::swap;
        
        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_list, __x._M_list);
        swap(_M_last, __x._M_last);
        swap(_M_count, __x._M_count);
    }

    void clear() noexcept
    {
        erase(begin(), end());
    }

    void splice(const_iterator __position, list& __x)
    {
        splice(__position, __x, __x.begin(), __x.end());
    }

    void splice(const_iterator __position, list&& __x)
    {
        splice(__position, __x);
    }

    void splice(const_iterator __position, list& __x, const_iterator __i)
    {
        if (__i != __x.end())
            splice(__position, __x, __i, next(__i));
    }

    void splice(const_iterator __position, list&& __x, const_iterator __i)
    {
        splice(__position, __x, __i);
    }

    void splice(const_iterator __position, list& __x, const_iterator __first, const_iterator __last)
    {
        // Optimisation for self splice.
        if (&__x == this)
        {
            auto* __pos = __position._M_element;
            auto* __f = __first._M_element;
            auto* __l = std::prev(__last)._M_element;
            auto* __prev = (__pos) ? __pos->__prev : _M_last;

            __f->__prev = __prev;
            __l->__next = __pos;

            if (__pos)
                __pos->__prev = __l;
            else
                _M_last = __l;

            if (__prev)
                __prev->__next = __f;
            else
                _M_list = __f;

            return;
        }
        
        // Optimisation for full-list splice.
        if (__first == __x.begin() && __last == __x.end())
        {
            if (__first == __last)
                return;
            
            auto* __pos = __position._M_element;
            auto* __f = __first._M_element;
            auto* __l = std::prev(__last)._M_element;
            auto* __prev = (__pos) ? __pos->__prev : _M_last;

            __f->__prev = __prev;
            __l->__next = __pos;

            if (__pos)
                __pos->__prev = __l;
            else
                _M_last = __l;

            if (__prev)
                __prev->__next = __f;
            else
                _M_list = __f;

            _M_count += __x._M_count;

            __x._M_list = nullptr;
            __x._M_last = nullptr;
            __x._M_count = 0;

            return;
        }
        
        // The splice needs to be done element-by-element so the count can be adjusted appropriately.
        while (__first != __last)
        {
            auto* __elem = __first._M_element;
            __x.__remove(__elem);
            __insert_before(__position._M_element, __elem);
        }
    }

    size_type remove(const _T& __value)
    {
        return remove_if([&__value](auto& __x) { return __x == __value; });
    }

    template <class _Predicate>
    size_type remove_if(_Predicate __pred)
    {
        size_type __n = 0;
        auto __i = begin();
        while (__i != end())
        {
            if (__pred(*__i))
            {
                __i = erase(__i);
                ++__n;
            }
            else
            {
                ++__i;
            }
        }

        return __n;
    }

    size_type unique()
    {
        return unique(std::equal_to{});
    }

    template <class _BinaryPredicate>
    size_type unique(_BinaryPredicate __pred)
    {
        if (begin() == end())
            return 0;
        
        size_type __n = 0;
        auto __prev = begin();
        auto __i = next(__prev);

        while (__i != end())
        {
            if (__pred(*__prev, *__i))
            {
                __i = erase(__i);
                ++__n;
            }
            else
            {
                __prev = __i;
                ++__i;
            }
        }

        return __n;
    }

    void merge(list& __x)
    {
        merge(__x, std::less{});
    }

    void merge(list&& __x)
    {
        merge(__x);
    }

    template <class _Compare>
    void merge(list& __x, _Compare __comp)
    {
        if (&__x == this)
            return;
        
        list __list;
        
        while (begin() != end() && __x.begin() != __x.end())
        {
            auto __i = begin();
            auto __j = __x.begin();

            if (__comp(*__j, *__i))
                __list.splice(__list.end(), __x, __j);
            else
                __list.splice(__list.end(), *this, __i);
        }

        // At most one of these will insert any elements.
        __list.splice(__list.end(), *this);
        __list.splice(__list.end(), __x);

        splice(begin(), __list);
    }

    template <class _Compare>
    void merge(list&& __x, _Compare __comp)
    {
        merge(__x, __comp);
    }

    void sort()
    {
        sort(std::less{});
    }

    template <class _Compare>
    void sort(_Compare __comp)
    {
        // Merge sort.

        // One- or zero-element lists aare trivially sorted.
        if (begin() == end() || next(begin()) == end())
            return;

        // Split into two lists arbitrarily.
        list __left = {};
        list __right = {};
        size_type __i = 0;
        while (begin() != end())
        {
            if (__i & 1)
                __left.splice(__left.end(), *this, begin());
            else
                __right.splice(__right.end(), *this, begin());
            ++__i;
        }

        // Recursively sort the sub-lists.
        __left.sort();
        __right.sort();

        // Merge the sorted lists.
        __left.merge(__right);
        
        splice(begin(), __left);
    }

    void reverse() noexcept
    {
        list __tmp;

        while (begin() != end())
            __tmp.splice(__tmp.begin(), *this, begin());

        splice(begin(), __tmp);
    }


private:

    using __element_alloc   = typename allocator_traits<_Allocator>::template rebind_alloc<__element>;
    using __element_traits  = allocator_traits<__element_alloc>;


    [[no_unique_address]] __element_alloc _M_allocator = {};

    __element*  _M_list     = nullptr;
    __element*  _M_last     = nullptr;
    size_type   _M_count    = 0;


    __element* __allocate_element()
    {
        return __element_traits::allocate(_M_allocator, 1);
    }

    void __deallocate_element(__element* __p)
    {
        __element_traits::deallocate(_M_allocator, __p, 1);
    }

    void __delete_element(__element* __p)
    {
        __element_traits::destroy(_M_allocator, __p);
        __deallocate_element(__p);
    }

    template <class... _Args>
    __element* __new_element(_Args&&... __args)
    {
        auto __p = __allocate_element();
        if (!__p)
            throw bad_alloc();

        // We don't want to construct an __element as there's no guarantee we can default-construct value_type. Instead,
        // do it piece-wise.
        try
        {
            _Allocator __a = _M_allocator;
            new (&__p->__next) __element*(nullptr);
            new (&__p->__prev) __element*(nullptr);
            allocator_traits<_Allocator>::construct(__a, &__p->__data, std::forward<_Args>(__args)...);
        }
        catch (...)
        {
            __deallocate_element(__p);
            throw;
        }

        return __p;
    }

    __element* __insert_after(__element* __pos, __element* __new)
    {
        __new->__next = __pos->__next;
        __new->__prev = __pos;

        if (__pos)
            __pos->__next = __new;
        else
            _M_list = __new;

        if (__new->__next)
            __new->__next->__prev = __new;
        else
            _M_last = __new;

        ++_M_count;

        return __new;
    }

    __element* __insert_before(__element* __pos, __element* __new)
    {
        if (__pos != nullptr)
            __pos = __pos->__prev;
        else
            __pos = _M_last;

        return __insert_after(__pos, __new);
    }

    __element* __remove(__element* __which)
    {
        if (__which->__prev)
            __which->__prev->__next = __which->__next;
        else
            _M_list = __which->__next;

        auto __next = __which->__next;
        if (__which->__next)
            __which->__next->__prev = __which->__prev;
        else
            _M_last = __which->__prev;

        __which->__next = nullptr;
        __which->__prev = nullptr;

        --_M_count;

        return __next;
    }

    void __remove_and_delete_element(__element* __which)
    {
        __remove(__which);
        __delete_element(__which);
    }

    template <class... _Args>
    __element* __insert_new_element_after(__element* __pos, _Args&&... __args)
    {
        auto __e = __new_element(std::forward<_Args>(__args)...);
        return __insert_after(__pos, __e);
    }

    template <class... _Args>
    __element* __insert_new_element_before(__element* __pos, _Args&&... __args)
    {
        auto __e = __new_element(std::forward<_Args>(__args)...);
        return __insert_before(__pos, __e);
    }
};

template <class _InputIterator, class _Allocator = allocator<__detail::__iter_value_t<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
list(_InputIterator, _InputIterator, _Allocator = _Allocator()) -> list<__detail::__iter_value_t<_InputIterator>, _Allocator>;


template <class _T, class _Allocator>
bool operator==(const list<_T, _Allocator>& __x, const list<_T, _Allocator>& __y)
{
    if (&__x == &__y)
        return true;

    if (__x.size() != __y.size())
        return false;

    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _T, class _Allocator>
__detail::__synth_three_way_result<_T> operator<=>(const list<_T, _Allocator>& __x, const list<_T, _Allocator>& __y)
{
    return lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}

template <class _T, class _Allocator>
void swap(list<_T, _Allocator>& __x, list<_T, _Allocator>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Allocator, class _U>
typename list<_T, _Allocator>::size_type erase(list<_T, _Allocator>& __c, const _U& __value)
{
    return erase_if(__c, [&__value](auto& __x) { return __x == __value; });
}

template <class _T, class _Allocator, class _Predicate>
typename list<_T, _Allocator>::size_type erase_if(list<_T, _Allocator>& __c, _Predicate __pred)
{
    return __c.remove_if(__pred);
}


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_LIST_H */

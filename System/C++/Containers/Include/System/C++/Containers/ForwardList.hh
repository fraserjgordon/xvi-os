#ifndef __SYSTEM_CXX_CONTAINERS_FORWARDLIST_H
#define __SYSTEM_CXX_CONTAINERS_FORWARDLIST_H


#include <System/C++/Containers/Private/Config.hh>
#include <System/C++/Containers/Private/Allocator.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/MemoryResource.hh>


namespace __XVI_STD_CONTAINERS_NS
{


template <class _T, class _Allocator = allocator<_T>>
class forward_list
{
public:

    template <bool _Const> class __iterator;
    struct __element;

    using value_type            = _T;
    using allocator_type        = _Allocator;
    using pointer               = typename allocator_traits<_Allocator>::pointer;
    using const_pointer         = typename allocator_traits<_Allocator>::const_pointer;
    using reference             = value_type&;
    using const_reference       = const value_type&;
    using size_type             = std::size_t;
    using difference_type       = std::ptrdiff_t;
    using iterator              = __iterator<false>;
    using const_iterator        = __iterator<true>;

    struct __element
    {
        __element*      __next  = nullptr;
        value_type      __data;
    };

    template <bool _Const>
    class __iterator
    {
    public:

        friend class forward_list;

        using value_type            = _T;
        using difference_type       = std::ptrdiff_t;
        using pointer               = _T*;
        using reference             = _T&;
        using iterator_category     = std::forward_iterator_tag;

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

        bool operator==(const __iterator& __x) const
        {
            return _M_element == __x._M_element;
        }

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

    private:

        __element*      _M_element = nullptr;

        explicit __iterator(__element* __e) :
            _M_element(__e)
        {
        }

        __iterator<!_Const> __convert()
            requires (_Const)
        {
            return __iterator<!_Const>(_M_element);
        }
    };


    forward_list() :
        forward_list(_Allocator())
    {
    }

    explicit forward_list(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    explicit forward_list(size_type __n, const _Allocator& __a = _Allocator()) :
        forward_list(__a)
    {
        resize(__n);
    }

    forward_list(size_type __n, const _T& __value, const _Allocator& __a = _Allocator()) :
        forward_list(__a)
    {
        assign(__n, __value);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    forward_list(_InputIterator __first, _InputIterator __last, const _Allocator& __a = _Allocator()) :
        forward_list(__a)
    {
        assign(__first, __last);
    }

    forward_list(const forward_list& __x) :
        forward_list(__x, __x._M_allocator)
    {
    }

    forward_list(forward_list&& __x) :
        forward_list(std::move(__x), __x._M_allocator)
    {
    }

    forward_list(const forward_list& __x, const _Allocator& __a) :
        forward_list(__a)
    {
        assign(__x.begin(), __x.end());
    }

    forward_list(forward_list&& __x, const _Allocator& __a) :
        forward_list(__a)
    {
        using std::swap;
        swap(_M_list, __x._M_list);
    }

    forward_list(initializer_list<_T> __il, const _Allocator& __a = _Allocator()) :
        forward_list(__a)
    {
        assign(__il);
    }

    ~forward_list()
    {
        clear();
    }

    forward_list& operator=(const forward_list& __x)
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

    forward_list& operator=(forward_list&& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        // No self-assign check; move done via swaps.
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_list, __x._M_list);
        return *this;
    }

    forward_list& operator=(initializer_list<_T> __il)
    {
        assign(__il);
        return *this;
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    void assign(_InputIterator __first, _InputIterator __last)
    {
        // Overwrite existing elements as much as possible.
        auto __i = begin();
        auto __prev = before_begin();
        auto __end = end();
        for (; __i != __end && __first != __last; ++__i, ++__prev, ++__first)
            *__i = *__first;

        // If we have elements left over, delete them.
        if (__i != __end)
        {
            erase_after(__prev, __end);
            return;
        }

        // Insert the remaining elements.
        insert_after(__prev, __first, __last);
    }

    void assign(size_type __n, const _T& __t)
    {
        // Overwrite existing elements as much as possible.
        auto __i = begin();
        auto __prev = before_begin();
        auto __end = end();
        for (; __i != __end && __n > 0; ++__i, ++__prev, --__n)
            *__i = __t;

        // If we have elements left over, delete them.
        if (__i != __end)
        {
            erase_after(__prev, __end);
            return;
        }

        // Insert the remaining elements.
        insert_after(__prev, __n, __t);
    }

    void assign(initializer_list<_T> __il)
    {
        assign(__il.begin(), __il.end());
    }

    allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    iterator before_begin() noexcept
    {
        return __before();
    }

    const_iterator before_begin() const noexcept
    {
        return __before();
    }

    iterator begin() noexcept
    {
        return iterator{_M_list};
    }

    const_iterator begin() const noexcept
    {
        return const_iterator{_M_list};
    }

    iterator end() noexcept
    {
        return iterator{nullptr};
    }

    const_iterator end() const noexcept
    {
        return const_iterator{nullptr};
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cbefore_begin() const noexcept
    {
        return before_begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return (_M_list == nullptr);
    }

    size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max() / sizeof(__element);
    }

    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    template <class... _Args>
    reference emplace_front(_Args&&... __args)
    {
        auto* __e = __new_element(std::forward<_Args>(__args)...);
        __e->__next = _M_list;
        _M_list = __e;

        return __e->__data;
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
        if (!_M_list)
            return;

        auto* __e = _M_list;
        _M_list = _M_list->__next;

        __delete_element(__e);
    }

    template <class... _Args>
    iterator emplace_after(const_iterator __position, _Args&&... __args)
    {
        auto* __e = __insert_new_element_after(__position._M_element, std::forward<_Args>(__args)...);
        return iterator{__e};
    }

    iterator insert_after(const_iterator __position, const _T& __x)
    {
        return emplace_after(__position, __x);
    }

    iterator insert_after(const_iterator __position, _T&& __x)
    {
        return emplace_after(__position, std::move(__x));
    }

    iterator insert_after(const_iterator __position, size_type __n, const _T& __x)
    {
        for (; __n > 0; --__n)
            __position = insert_after(__position, __x);
        return __position.__convert();
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    iterator insert_after(const_iterator __position, _InputIterator __first, _InputIterator __last)
    {
        for (; __first != __last; ++__first)
            __position = emplace_after(__position, *__first);
        return __position.__convert();
    }

    iterator insert_after(const_iterator __position, initializer_list<_T> __il)
    {
        return insert_after(__position, __il.begin(), __il.end());
    }

    iterator erase_after(const_iterator __position)
    {
        auto* __after = __position._M_element;

        auto* __e = __after->__next;
        __after->__next = __e->__next;
        __delete_element(__e);

        return iterator(__after->__next);
    }

    iterator erase_after(const_iterator __position, const_iterator __last)
    {
        auto* __after = __position._M_element;
        auto* __until = __last._M_element;

        while (__after->__next != __until)
            erase_after(iterator{__after});

        return __last.__convert();
    }

    void swap(forward_list& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_list, __x._M_list);
    }

    void resize(size_type __sz)
    {
        auto __first = begin();
        auto __prev = before_begin();
        auto __last = end();

        // Skip (up to) the initial __sz elements.
        for(; __first != __last && __sz > 0; ++__first, ++__prev, --__sz)
            ;

        // Is the new size bigger than the old size?
        if (__sz > 0)
        {
            for (; __sz > 0; --__sz)
                __prev = emplace_after(__prev);
        }
        else
        {
            erase_after(__prev, __last);
        }
    }

    void resize(size_type __sz, const value_type& __c)
    {
        auto __first = begin();
        auto __prev = before_begin();
        auto __last = end();

        // Skip (up to) the initial __sz elements.
        for (; __first != __last && __sz > 0; ++__first, ++__prev, --__sz)
            ;

        // Is the new size bigger than the old size?
        if (__sz > 0)
            insert_after(__prev, __sz, __c);
        else
            erase_after(__prev, __last);
    }

    void clear() noexcept
    {
        erase_after(begin(), end());
    }

    void splice_after(const_iterator __position, forward_list& __x)
    {
        auto* __pos = __position._M_element;

        auto* __tail = __pos->__next;
        __pos->__next = __x._M_list;
        __x._M_list = nullptr;

        auto* __i = __pos;
        for (; __i->__next != nullptr; ++__i)
            ;

        __i->__next = __tail;
    }

    void splice_after(const_iterator __position, forward_list&& __x)
    {
        splice_after(__position, __x);
    }

    void splice_after(const_iterator __position, forward_list&, const_iterator __i)
    {
        auto* __pos = __position._M_element;
        auto* __e = __i._M_element;

        auto* __tail = __pos->__next;
        __pos->__next = __e->__next;
        __e->__next = __e->__next->__next;
        __pos->__next->__next = __tail;
    }

    void splice_after(const_iterator __position, forward_list&& __x, const_iterator __i)
    {
        splice_after(__position, __x, __i);
    }

    void splice_after(const_iterator __position, forward_list& __x, const_iterator __first, const_iterator __last)
    {
        auto* __pos = __position._M_element;
        auto* __f = __first._M_element;
        auto* __l = __last._M_element;

        auto* __tail = __pos->__next;
        __pos->__next = __f->__next;
        __f->__next = __l;

        auto* __i = __pos;
        for (; __i->__next != __last._M_element; __i = __i->__next)
            ;

        __i->__next = __tail;
    }

    void splice_after(const_iterator __position, forward_list&& __x, const_iterator __first, const_iterator __last)
    {
        splice_after(__position, __x, __first, __last);
    }

    size_type remove(const _T& __value)
    {
        return remove_if([&__value](auto& __x) { return __x == __value; });
    }

    template <class _Predicate>
    size_type remove_if(_Predicate __pred)
    {
        size_type __n = 0;
        for (auto __i = begin(), __prev = before_begin(); __i != end(); ++__i, ++__prev)
        {
            if (__pred(*__i))
            {
                erase_after(__prev);
                ++__n;
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
        size_type __n = 0;
        for (auto __i = begin(), __prev = before_begin(); __i != end(); ++__i)
        {
            // Nothing to do on the first iteration.
            if (__prev == before_begin())
                continue;

            while (__pred(*__prev, *__i))
            {
                __i = erase_after(__prev);
                ++__n;
            }

            __prev = __i;
        }

        return __n;
    }

    void merge(forward_list& __x)
    {
        merge(__x, std::less{});
    }

    void merge(forward_list&& __x)
    {
        merge(__x);
    }

    template <class _Compare>
    void merge(forward_list& __x, _Compare __comp)
    {
        forward_list __list;
        auto __ibegin = before_begin();
        auto __jbegin = __x.before_begin();
        auto __out = __list.before_begin();

        while (begin() != end() && __x.begin() != __x.end())
        {
            auto __i = begin();
            auto __j = __x.begin();
            
            if (__comp(*__j, *__i))
                __list.splice_after(__out, __x, __jbegin);
            else
                __list.splice_after(__out, *this, __ibegin);

            ++__out;
        }

        // At most one of these will insert any elements.
        __list.splice_after(__out, __x, __jbegin);
        __list.splice_after(__out, *this, __ibegin);

        splice_after(__ibegin, __list);
    }

    template <class _Compare>
    void merge(forward_list&& __x, _Compare __comp)
    {
        merge(__x, std::move(__comp));
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
        forward_list __left = {};
        forward_list __right = {};
        size_type __i = 0;
        while (begin() != end())
        {
            if (__i & 1)
                __left.splice_after(__left.before_begin(), *this, before_begin());
            else
                __right.splice_after(__right.before_begin(), *this, before_begin());
            ++__i;
        }

        // Recursively sort the sub-lists.
        __left.sort();
        __right.sort();

        // Merge the sorted lists.
        __left.merge(__right);
        splice_after(before_begin(), __left, __left.before_begin());
    }

    void reverse() noexcept
    {
        forward_list __temp = {};

        while (begin() != end())
            __temp.splice_after(__temp.before_begin(), *this, before_begin());

        splice_after(before_begin(), __temp);
    }


private:

    using __element_alloc   = typename allocator_traits<_Allocator>::template rebind_alloc<__element>;
    using __element_traits  = allocator_traits<__element_alloc>;


    [[no_unique_address]] __element_alloc _M_allocator = {};

    __element* _M_list = nullptr;


    iterator __before() const
    {
        // Casts the _M_list member as if it was the first field in an __element object.
        auto __element_pp = const_cast<__element**>(&_M_list);
        auto __fake_element = reinterpret_cast<__element*>(__element_pp - 1);

        return iterator{__fake_element};
    }

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
        __pos->__next = __new;
        return __new;
    }

    template <class... _Args>
    __element* __insert_new_element_after(__element* __pos, _Args&&... __args)
    {
        auto __e = __new_element(std::forward<_Args>(__args)...);
        return __insert_after(__pos, __e);
    }
};

template <class _InputIterator, class _Allocator = allocator<__detail::__iter_value_t<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator>
          && __detail::__is_allocator<_Allocator>
forward_list(_InputIterator, _InputIterator, _Allocator = _Allocator()) -> forward_list<_InputIterator, _Allocator>;


template <class _T, class _Allocator>
bool operator==(const forward_list<_T, _Allocator>& __x, const forward_list<_T, _Allocator>& __y)
{
    return equal(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class _T, class _Allocator>
__detail::__synth_three_way_result<_T> operator<=>(const forward_list<_T, _Allocator>& __x, const forward_list<_T, _Allocator>& __y)
{
    return lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}

template <class _T, class _Allocator>
void swap(forward_list<_T, _Allocator>& __x, forward_list<_T, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Allocator, class _U>
typename forward_list<_T, _Allocator>::size_type erase(forward_list<_T, _Allocator>& __c, const _U& __value)
{
    return erase_if(__c, [&__value](auto& __elem) { return __elem == __value; });
}

template <class _T, class _Allocator, class _Pred>
typename forward_list<_T, _Allocator>::size_type erase_if(forward_list<_T, _Allocator>& __c, _Pred __predicate)
{
    return __c.remove_if(__predicate);
}


namespace pmr
{

template <class _T>
using forward_list = __XVI_STD_CONTAINERS_NS::forward_list<_T, pmr::polymorphic_allocator<_T>>;

} // namespace pmr


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_FORWARDLIST_H */

#ifndef __SYSTEM_CXX_CONTAINERS_SET_H
#define __SYSTEM_CXX_CONTAINERS_SET_H


#include <System/C++/Containers/Private/Config.hh>
#include <System/C++/Containers/Private/Allocator.hh>
#include <System/C++/Containers/Private/RBTree.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/MemoryResource.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace __XVI_STD_CONTAINERS_NS
{


// Forward declarations.
template <class _Key, class _Compare = less<_Key>, class _Allocator = allocator<_Key>> class set;
template <class _Key, class _Compare = less<_Key>, class _Allocator = allocator<_Key>> class multiset;


namespace __detail
{

template <class _Key, class _Allocator>
class __set_node_handle
{
public:

    using value_type        = _Key;
    using allocator_type    = _Allocator;

    constexpr __set_node_handle() noexcept = default;

    constexpr __set_node_handle(__set_node_handle&& __x) :
        _M_ptr(std::move(__x._M_ptr)),
        _M_allocator(std::move((__x._M_allocator)))
    {
        __x._M_ptr = nullptr;
        __x._M_allocator = nullopt;
    }

    constexpr __set_node_handle& operator=(__set_node_handle&& __x)
    {
        if (_M_ptr)
            __rbtree::__delete_node(*_M_allocator, _M_ptr);

        _M_ptr = __x._M_ptr;

        if (!_M_allocator || __ator_traits::propagage_on_container_move_assignment::value)
            _M_allocator = std::move(__x._M_allocator);

        __x._M_ptr = nullptr;
        __x._M_allocator = nullopt;

        return *this;
    }

    __set_node_handle(const __set_node_handle&) = delete;
    __set_node_handle& operator=(const __set_node_handle&) = delete;

    ~__set_node_handle()
    {
        if (!_M_ptr)
            return;

        __rbtree::__delete_node(*_M_allocator, _M_ptr);
    }

    value_type& value() const
    {
        return *_M_ptr->__data();
    }

    allocator_type get_allocator() const
    {
        return *_M_allocator;
    }

    explicit operator bool() const noexcept
    {
        return _M_ptr != nullptr;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return _M_ptr == nullptr;
    }

    void swap(__set_node_handle& __x)
        noexcept(__ator_traits::propagate_on_container_swap::value || __ator_traits::is_always_equal::value)
    {
        swap(_M_ptr, __x._M_ptr);

        if (!_M_allocator || !__x._M_allocator || __ator_traits::propagate_on_container_swap::value)
            swap(_M_allocator, __x._M_allocator);
    }

private:

    using __rbtree              = __detail::__rbtree<value_type>;
    using __container_node_type = __rbtree::__node;
    using __ator                = typename allocator_traits<allocator_type>::template rebind_alloc<__container_node_type>;
    using __ator_traits         = allocator_traits<__ator>;
    using __ptr_type            = typename __ator_traits::pointer;

    __ptr_type                  _M_ptr  = nullptr;
    optional<__ator>            _M_allocator = nullopt;


    template <class, class, class> friend class __XVI_STD_CONTAINERS_NS::set;
    template <class, class, class> friend class __XVI_STD_CONTAINERS_NS::multiset;


    constexpr __set_node_handle(__ptr_type __ptr, allocator_type __a) :
        _M_ptr(__ptr), _M_allocator(std::move(__a))
    {
    }
};

} // namespace __detail


template <class _Key, class _Compare, class _Allocator>
class set
{
public:

    using __rbtree = __detail::__rbtree<_Key>;

    using key_type                  = _Key;
    using key_compare               = _Compare;
    using value_type                = _Key;
    using value_compare             = _Compare;
    using allocator_type            = _Allocator;
    using pointer                   = typename allocator_traits<_Allocator>::pointer;
    using const_pointer             = typename allocator_traits<_Allocator>::const_pointer;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = typename __rbtree::template __iterator<false, true>;
    using const_iterator            = typename __rbtree::template __iterator<true, true>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using node_type                 = __detail::__set_node_handle<_Key, _Allocator>;
    using insert_return_type        = __detail::__insert_return_type<iterator, node_type>;

    set() :
        set(_Compare())
    {
    }

    explicit set(const _Compare& __comp, const _Allocator& __a = _Allocator()) :
        _M_allocator(__a), _M_compare(__comp)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    set(_InputIterator __first, _InputIterator __last, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        set(__comp, __a)
    {
        insert(__first, __last);
    }

    set(const set& __x) :
        _M_allocator(__x._M_allocator),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    set(set&& __x) :
        _M_allocator(std::move(__x._M_allocator)),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    explicit set(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    set(const set& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    set(set&& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    set(initializer_list<value_type> __il, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        set(__il.begin(), __il.end(), __comp, __a)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    set(_InputIterator __first, _InputIterator __last, const _Allocator& __a) :
        set(__first, __last, _Compare(), __a)
    {
    }

    set(initializer_list<value_type> __il, const _Allocator& __a) :
        set(__il, _Compare(), __a)
    {
    }

    ~set()
    {
        clear();
    }

    set& operator=(const set& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;

        clear();

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            _M_allocator = __x._M_allocator;

        _M_compare = __x._M_compare;
        insert(__x.begin(), __x.end());

        return *this;
    }

    set& operator=(set&& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value && is_nothrow_move_assignable_v<_Compare>)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_compare, __x._M_compare);
        swap(_M_tree, __x._M_tree);

        return *this;
    }

    set& operator=(initializer_list<value_type> __il)
    {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    iterator begin() noexcept
    {
        return _M_tree.__begin();
    }

    const_iterator begin() const noexcept
    {
        return _M_tree.__begin();
    }

    iterator end() noexcept
    {
        return _M_tree.__end();
    }

    const_iterator end() const noexcept
    {
        return _M_tree.__end();
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
        return _M_tree.__size();
    }

    size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max() / sizeof(typename __rbtree::__node);
    }

    template <class... _Args>
    pair<iterator, bool> emplace(_Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        if (auto __i = _M_tree.__insert_unique(__n, *__n->__data(), key_comp()); __i != __n)
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return {__make_iter(__i), false};
        }

        return {__make_iter(__n), true};
    }

    template <class... _Args>
    iterator emplace_hint(const_iterator __hint, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        if (auto __i = _M_tree.__insert_unique_hint(__hint.__ptr(), __n, *__n->__data(), key_comp()); __i != __n)
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return __make_iter(__n);
        }

        return __make_iter(__n);
    }

    pair<iterator, bool> insert(const value_type& __x)
    {
        return emplace(__x);
    }

    pair<iterator, bool> insert(value_type&& __x)
    {
        return emplace(std::move(__x));
    }

    iterator insert(const_iterator __hint, const value_type __x)
    {
        return emplace_hint(__hint, __x);
    }

    iterator insert(const_iterator __hint, value_type&& __x)
    {
        return emplace_hint(__hint, std::move(__x));
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    void insert(_InputIterator __first, _InputIterator __last)
    {
        for (; __first != __last; ++__first)
            emplace(*__first);
    }

    void insert(initializer_list<value_type> __il)
    {
        insert(__il.begin(), __il.end());
    }

    node_type extract(const_iterator __position)
    {
        auto __n = __position.__ptr();
        _M_tree.__extract(__n);

        return node_type(__n, _M_allocator);
    }

    node_type extract(const key_type& __k)
    {
        auto __i = find(__k);
        if (__i == end())
            return {};

        return extract(__i);
    }

    insert_return_type insert(node_type&& __nh)
    {
        if (!__nh)
            return {end(), false, {}};

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_unique(__n, *__n->__data(), key_comp());
        if (__i != __n)
            return {__make_iter(__i), false, std::move(__nh)};

        __nh._M_ptr = nullptr;
        return {__make_iter(__n), true, {}};
    }

    iterator insert(const_iterator __hint, node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_unique_hint(__hint.__ptr(), __n, *__n->__data(), key_comp());
        if (__i != __n)
            return __make_iter(__i);

        __nh._M_ptr = nullptr;
        return __make_iter(__n);
    }

    iterator erase(iterator __position)
    {
        auto __n = __position.__ptr();
        auto __i = std::next(__position);

        _M_tree.__extract(__n);

        __rbtree::__delete_node(_M_allocator, __n);
        return __i;
    }

    iterator erase(const_iterator __position)
    {
        auto __n = __position.__ptr();
        auto __i = std::next(__make_iter(__n));

        _M_tree.__extract(__n);

        __rbtree::__delete_node(_M_allocator, __n);
        return __i;
    }

    size_type erase(const key_type& __k)
    {
        auto [__first, __last] = equal_range(__k);
        auto __s = std::distance(__first, __last);

        erase(__first, __last);

        return __s;
    }

    iterator erase(const_iterator __first, const_iterator __last)
    {
        while (__first != __last)
            __first = erase(__first);

        return __make_iter(__last.__ptr());
    }

    void swap(set& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value && is_nothrow_swappable_v<_Compare>)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_compare, __x._M_compare);
        swap(_M_tree, __x._M_tree);
    }

    void clear() noexcept
    {
        erase(begin(), end());
    }

    template <class _C2>
    void merge(set<_Key, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            auto [__i, __where] = _M_tree.__find_insert_point(*__b, key_comp());
            if (__where != __rbtree::__node::__where::__here)
                insert(__make_iter(__i), __source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(set<_Key, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    template <class _C2>
    void merge(multiset<_Key, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            auto [__i, __where] = _M_tree.__find_insert_point(*__b, key_comp());
            if (__where != __rbtree::__node::__where::__here)
                insert(__make_iter(__i), __source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(multiset<_Key, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    key_compare key_comp() const
    {
        return _M_compare;
    }

    value_compare value_comp() const
    {
        return _M_compare;
    }

    iterator find(const key_type& __k)
    {
        return __make_iter(_M_tree.__find(__k, key_comp()));
    }

    const_iterator find(const key_type& __k) const
    {
        return __make_iter(_M_tree.__find(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator find(const _K& __k)
    {
        return __make_iter(_M_tree.__find(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator find(const _K& __k) const
    {
        return __make_iter(_M_tree.__find(__k, key_comp()));
    }

    size_type count(const key_type& __k) const
    {
        auto [__b, __e] = equal_range(__k);
        return std::distance(__b, __e);
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    size_type count(const _K& __k) const
    {
        auto [__b, __e] = equal_range(__k);
        return std::distance(__b, __e);
    }

    bool contains(const key_type& __k) const
    {
        return find(__k) != end();
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    bool contains(const _K& __k) const
    {
        return find(__k) != end();
    }

    iterator lower_bound(const key_type& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_comp()));
    }

    const_iterator lower_bound(const key_type& __k) const
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator lower_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator lower_bound(const _K& __k)  const
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_comp()));
    }

    iterator upper_bound(const key_type& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_comp()));
    }

    const_iterator upper_bound(const key_type& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator upper_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_comp()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator upper_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_comp()));
    }

    pair<iterator, iterator> equal_range(const key_type& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_comp());
        return {__make_iter(__b), __make_iter(__e)};
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_comp());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<iterator, iterator> equal_range(const _K& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_comp());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<const_iterator, const_iterator> equal_range(const _K& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_comp());
        return {__make_iter(__b), __make_iter(__e)};
    }

private:

    using __node_alloc = typename allocator_traits<_Allocator>::template rebind_alloc<typename __rbtree::__node>;

    [[no_unique_address]] __node_alloc  _M_allocator = {};
    [[no_unique_address]] _Compare      _M_compare = {};

    __rbtree _M_tree = {};


    constexpr iterator __make_iter(__rbtree::__node* __n)
    {
        return __n ? _M_tree.__make_iter(__n) : end();
    }

    constexpr const_iterator __make_iter(__rbtree::__node* __n) const
    {
        return __n ? _M_tree.__make_iter(__n) : end();
    }
};

template <class _InputIterator, class _Compare = less<__detail::__iter_value_type<_InputIterator>>, class _Allocator = allocator<__detail::__iter_value_type<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
set(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
    -> set<__detail::__iter_value_type<_InputIterator>, _Compare, _Allocator>;

template <class _Key, class _Compare = less<_Key>, class _Allocator = allocator<_Key>>
    requires __detail::__is_allocator<_Allocator>
set(initializer_list<_Key>, _Compare = _Compare(), _Allocator = _Allocator())
    -> set<_Key, _Compare, _Allocator>;

template <class _InputIterator, class _Allocator>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
set(_InputIterator, _InputIterator, _Allocator)
    -> set<__detail::__iter_value_type<_InputIterator>, less<__detail::__iter_value_type<_InputIterator>>, _Allocator>;

template <class _Key, class _Allocator>
    requires __detail::__is_allocator<_Allocator>
set(initializer_list<_Key>, _Allocator)
    -> set<_Key, less<_Key>, _Allocator>;


template <class _Key, class _Compare, class _Allocator>
bool operator==(const set<_Key, _Compare, _Allocator>& __x, const set<_Key, _Compare, _Allocator>& __y)
{
    if (&__x == &__y)
        return true;

    if (__x.size() != __y.size())
        return false;

    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Allocator>
__detail::__synth_three_way_result<_Key> operator<=>(const set<_Key, _Compare, _Allocator>& __x, const set<_Key, _Compare, _Allocator>& __y)
{
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}


template <class _Key, class _Compare, class _Allocator>
void swap(set<_Key, _Compare, _Allocator>& __x, set<_Key, _Compare, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Key, class _Compare, class _Allocator, class _Predicate>
typename set<_Key, _Compare, _Allocator>::size_type erase_if(set<_Key, _Compare, _Allocator>& __c, _Predicate __pred)
{
    auto __original_size = __c.size();
    for (auto __i = __c.begin(), __last = __c.end(); __i != __last; )
    {
        if (__pred(*__i))
            __i = __c.erase(__i);
        else
            ++__i;
    }

    return __original_size - __c.size();
}


template <class _Key, class _Compare, class _Allocator>
class multiset
{
public:

    using __rbtree = __detail::__rbtree<_Key>;

    using key_type                  = _Key;
    using key_compare               = _Compare;
    using value_type                = _Key;
    using value_compare             = _Compare;
    using allocator_type            = _Allocator;
    using pointer                   = typename allocator_traits<_Allocator>::pointer;
    using const_pointer             = typename allocator_traits<_Allocator>::const_pointer;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = typename __rbtree::template __iterator<false, true>;
    using const_iterator            = typename __rbtree::template __iterator<true, true>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using node_type                 = __detail::__set_node_handle<_Key, _Allocator>;

    multiset() :
        multiset(_Compare())
    {
    }

    explicit multiset(const _Compare& __comp, const _Allocator& __a = _Allocator()) :
        _M_allocator(__a), _M_compare(__comp)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    multiset(_InputIterator __first, _InputIterator __last, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        multiset(__comp, __a)
    {
        insert(__first, __last);
    }

    multiset(const multiset& __x) :
        _M_allocator(__x._M_allocator),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    multiset(multiset&& __x) :
        _M_allocator(std::move(__x._M_allocator)),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    explicit multiset(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    multiset(const multiset& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    multiset(multiset&& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    multiset(initializer_list<value_type> __il, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        multiset(__il.begin(), __il.end(), __comp, __a)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    multiset(_InputIterator __first, _InputIterator __last, const _Allocator& __a) :
        multiset(__first, __last, _Compare(), __a)
    {
    }

    multiset(initializer_list<value_type> __il, const _Allocator& __a) :
        multiset(__il, _Compare(), __a)
    {
    }

    ~multiset()
    {
        clear();
    }

    multiset& operator=(const multiset& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;

        clear();

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            _M_allocator = __x._M_allocator;

        _M_compare = __x._M_compare;
        insert(__x.begin(), __x.end());

        return *this;
    }

    multiset& operator=(multiset&& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value && is_nothrow_move_assignable_v<_Compare>)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_move_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_compare, __x._M_compare);
        swap(_M_tree, __x._M_tree);

        return *this;
    }

    multiset& operator=(initializer_list<value_type> __il)
    {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    allocator_type get_allocator() const noexcept
    {
        return _M_allocator;
    }

    iterator begin() noexcept
    {
        return _M_tree.__begin();
    }

    const_iterator begin() const noexcept
    {
        return _M_tree.__begin();
    }

    iterator end() noexcept
    {
        return _M_tree.__end();
    }

    const_iterator end() const noexcept
    {
        return _M_tree.__end();
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
        return _M_tree.__size();
    }

    size_type max_size() const noexcept
    {
        return numeric_limits<size_type>::max() / sizeof(typename __rbtree::__node);
    }

    template <class... _Args>
    iterator emplace(_Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        return __make_iter(_M_tree.__insert_nonunique(__n, *__n->__data(), key_comp()));
    }

    template <class... _Args>
    iterator emplace_hint(const_iterator __hint, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        return __make_iter(_M_tree.__insert_nonunique_hint(__hint.__ptr(), __n, *__n->__data(), key_comp()));
    }

    iterator insert(const value_type& __x)
    {
        return emplace(__x);
    }

    iterator insert(value_type&& __x)
    {
        return emplace(std::move(__x));
    }

    iterator insert(const_iterator __hint, const value_type& __x)
    {
        return emplace_hint(__hint, __x);
    }

    iterator insert(const_iterator __hint, value_type&& __x)
    {
        return emplace_hint(__hint, std::move(__x));
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    void insert(_InputIterator __first, _InputIterator __last)
    {
        for (; __first != __last; ++__first)
            emplace(*__first);
    }

    void insert(initializer_list<value_type> __il)
    {
        insert(__il.begin(), __il.end());
    }

    node_type extract(const_iterator __position)
    {
        auto __n = __position.__ptr();
        _M_tree.__extract(__n);

        return node_type(__n, _M_allocator);
    }

    node_type extract(const key_type& __k)
    {
        auto __i = lower_bound(__k);
        if (__i == end() || key_comp()(__k, *__i))
            return {};

        return extract(__i);
    }

    iterator insert(node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_nonunique(__n, *__n->__data(), key_comp());

        __nh._M_ptr = nullptr;
        return __make_iter(__i);
    }

    iterator insert(const_iterator __hint, node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_nonunique_hint(__hint.__ptr(), __n, *__n->__data(), key_comp());

        __nh._M_ptr = nullptr;
        return __make_iter(__i);
    }

    iterator erase(iterator __position)
    {
        auto __n = __position.__ptr();
        auto __i = std::next(__position);

        _M_tree.__extract(__n);

        __rbtree::__delete_node(_M_allocator, __n);
        return __i;
    }

    iterator erase(const_iterator __position)
    {
        auto __n = __position.__ptr();
        auto __i = std::next(__make_iter(__n));

        _M_tree.__extract(__n);

        __rbtree::__delete_node(_M_allocator, __n);

        return __i;
    }

    size_type erase(const key_type& __k)
    {
        auto [__first, __last] = equal_range(__k);
        auto __s = std::distance(__first, __last);

        erase(__first, __last);

        return __s;
    }

    iterator erase(const_iterator __first, const_iterator __last)
    {
        while (__first != __last)
            __first = erase(__first);

        return __make_iter(__last.__ptr());
    }

    void swap(multiset& __x)
        noexcept(allocator_traits<_Allocator>::is_always_equal::value && is_nothrow_swappable_v<_Compare>)
    {
        using std::swap;

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_swap::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
            swap(_M_allocator, __x._M_allocator);

        swap(_M_compare, __x._M_compare);
        swap(_M_tree, __x._M_tree);
    }

    void clear() noexcept
    {
        erase(begin(), end());
    }

    template <class _C2>
    void merge(multiset<_Key, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            insert(__source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(multiset<_Key, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    template <class _C2>
    void merge(set<_Key, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            insert(__source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(set<_Key, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    key_compare key_comp() const
    {
        return _M_compare;
    }

    value_compare value_comp() const
    {
        return _M_compare;
    }

    iterator find(const key_type& __k)
    {
        return __make_iter(_M_tree.__find(__k, key_compare()));
    }

    const_iterator find(const key_type& __k) const
    {
        return __make_iter(_M_tree.__find(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator find(const _K& __k)
    {
        return __make_iter(_M_tree.__find(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator find(const _K& __k) const
    {
        return __make_iter(_M_tree.__find(__k, key_compare()));
    }

    size_type count(const key_type& __k) const
    {
        auto [__b, __e] = equal_range(__k);
        return std::distance(__b, __e);
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    size_type count(const _K& __k) const
    {
        auto [__b, __e] = equal_range(__k);
        return std::distance(__b, __e);
    }

    bool contains(const key_type& __k) const
    {
        return find(__k) != end();
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    bool contains(const _K& __k) const
    {
        return find(__k) != end();
    }

    iterator lower_bound(const key_type& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_compare()));
    }

    const_iterator lower_bound(const key_type& __k) const
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator lower_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator lower_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__lower_bound(__k, key_compare()));
    }

    iterator upper_bound(const key_type& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_compare()));
    }

    const_iterator upper_bound(const key_type& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator upper_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator upper_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, key_compare()));
    }

    pair<iterator, iterator> equal_range(const key_type& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<iterator, iterator> equal_range(const _K& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<const_iterator, const_iterator> equal_range(const _K& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, key_compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

private:

    using __node_alloc = typename allocator_traits<_Allocator>::template rebind_alloc<typename __rbtree::__node>;

    [[no_unique_address]] __node_alloc  _M_allocator    = {};
    [[no_unique_address]] _Compare      _M_compare      = {};

    __rbtree _M_tree = {};


    constexpr iterator __make_iter(__rbtree::__node* __n)
    {
        return __n ? _M_tree.__make_iter(__n) : end();
    }

    constexpr const_iterator __make_iter(__rbtree::__node* __n) const
    {
        return __n ? _M_tree.__make_iter(__n) : end();
    }
};

template <class _InputIterator, class _Compare = less<__detail::__iter_value_type<_InputIterator>>, class _Allocator = allocator<__detail::__iter_value_type<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
multiset(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
    -> multiset<__detail::__iter_value_type<_InputIterator>, _Compare, _Allocator>;

template <class _Key, class _Compare = less<_Key>, class _Allocator = allocator<_Key>>
multiset(initializer_list<_Key>, _Compare = _Compare(), _Allocator = _Allocator())
    -> multiset<_Key, _Compare, _Allocator>;

template <class _InputIterator, class _Allocator>
multiset(_InputIterator, _InputIterator, _Allocator)
    -> multiset<__detail::__iter_value_type<_InputIterator>, less<__detail::__iter_value_type<_InputIterator>>, _Allocator>;

template <class _Key, class _Allocator>
multiset(initializer_list<_Key>, _Allocator)
    -> multiset<_Key, less<_Key>, _Allocator>;


template <class _Key, class _Compare, class _Allocator>
bool operator==(const multiset<_Key, _Compare, _Allocator>& __x, const multiset<_Key, _Compare, _Allocator>& __y)
{
    if (&__x == &__y)
        return true;

    if (__x.size() != __y.size())
        return false;

    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Allocator>
__detail::__synth_three_way_result<_Key> operator<=>(const multiset<_Key, _Compare, _Allocator>& __x, const multiset<_Key, _Compare, _Allocator>& __y)
{
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}


template <class _Key, class _Compare, class _Allocator>
void swap(multiset<_Key, _Compare, _Allocator>& __x, multiset<_Key, _Compare, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Key, class _Compare, class _Allocator, class _Predicate>
typename multiset<_Key, _Compare, _Allocator>::size_type erase_if(multiset<_Key, _Compare, _Allocator>& __c, _Predicate __pred)
{
    auto __original_size = __c.size();
    for (auto __i = __c.begin(), __last = __c.end(); __i != __last; )
    {
        if (__pred(*__i))
            __i = __c.erase(*__i);
        else
            ++__i;
    }

    return __c.size() - __original_size;
}


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_SET_H */

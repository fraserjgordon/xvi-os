#ifndef __SYSTEM_CXX_CONTAINERS_MAP_H
#define __SYSTEM_CXX_CONTAINERS_MAP_H


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
template <class _Key, class _T, class _Compare = less<_Key>, class _Allocator = allocator<pair<const _Key, _T>>> class map;
template <class _Key, class _T, class _Compare = less<_Key>, class _Allocator = allocator<pair<const _Key, _T>>> class multimap;


namespace __detail
{


template <class _Key, class _T, class _Allocator>
class __map_node_handle
{
public:

    using key_type              = _Key;
    using mapped_type           = _T;
    using allocator_type        = _Allocator;

    constexpr __map_node_handle() noexcept = default;

    constexpr __map_node_handle(__map_node_handle&& __x) :
        _M_ptr(std::move(__x._M_ptr)),
        _M_allocator(std::move((__x._M_allocator)))
    {
        __x._M_ptr = nullptr;
        __x._M_allocator = nullopt;
    }

    constexpr __map_node_handle& operator=(__map_node_handle&& __x)
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

    __map_node_handle(const __map_node_handle&) = delete;
    __map_node_handle& operator=(const __map_node_handle&) = delete;

    ~__map_node_handle()
    {
        if (!_M_ptr)
            return;

        __rbtree::__delete_node(*_M_allocator, _M_ptr);
    }

    key_type& key() const
    {
        return _M_ptr->__data()->first;
    }

    mapped_type& mapped() const
    {
        return _M_ptr->__data()->second;
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

    void swap(__map_node_handle& __x)
        noexcept(__ator_traits::propagate_on_container_swap::value || __ator_traits::is_always_equal::value)
    {
        swap(_M_ptr, __x._M_ptr);

        if (!_M_allocator || !__x._M_allocator || __ator_traits::propagate_on_container_swap::value)
            swap(_M_allocator, __x._M_allocator);
    }

private:

    using __rbtree              = __detail::__rbtree<pair<const _Key, _T>>;
    using __container_node_type = __rbtree::__node;
    using __ator                = typename allocator_traits<allocator_type>::template rebind_alloc<__container_node_type>;
    using __ator_traits         = allocator_traits<__ator>;
    using __ptr_type            = typename __ator_traits::pointer;

    __ptr_type                  _M_ptr  = nullptr;
    optional<__ator>            _M_allocator = nullopt;


    template <class, class, class, class> friend class __XVI_STD_CONTAINERS_NS::map;
    template <class, class, class, class> friend class __XVI_STD_CONTAINERS_NS::multimap;


    constexpr __map_node_handle(__ptr_type __ptr, allocator_type __a) :
        _M_ptr(__ptr), _M_allocator(std::move(__a))
    {
    }
};


} // namespace __detail


template <class _Key, class _T, class _Compare, class _Allocator>
class map
{
public:

    class value_compare;

    using __rbtree = __detail::__rbtree<pair<const _Key, _T>>;

    using key_type                  = _Key;
    using mapped_type               = _T;
    using value_type                = pair<const _Key, _T>;
    using key_compare               = _Compare;
    using allocator_type            = _Allocator;
    using pointer                   = typename allocator_traits<_Allocator>::pointer;
    using const_pointer             = typename allocator_traits<_Allocator>::const_pointer;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = typename __rbtree::template __iterator<false>;
    using const_iterator            = typename __rbtree::template __iterator<true>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using node_type                 = __detail::__map_node_handle<_Key, _T, _Allocator>;
    using insert_return_type        = __detail::__insert_return_type<iterator, node_type>;

    class value_compare
    {
        friend class map;

    protected:

        _Compare _M_comp;

        value_compare(_Compare __c) :
            _M_comp(__c)
        {
        }

    public:

        bool operator()(const value_type& __x, const value_type& __y) const
        {
            return _M_comp(__x.first, __y.first);
        }
    };


    map() :
        map(_Compare())
    {
    }

    explicit map(const _Compare& __comp, const _Allocator& __a = _Allocator()) :
        _M_allocator(__a), _M_compare(__comp)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    map(_InputIterator __first, _InputIterator __last, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        map(__comp, __a)
    {
        insert(__first, __last);
    }

    map(const map& __x) :
        _M_allocator(__x._M_allocator),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    map(map&& __x) :
        _M_allocator(std::move(__x._M_allocator)),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    explicit map(_Allocator& __a) :
        _M_allocator(__a)
    {
    }

    map(const map& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    map(map&& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    map(initializer_list<value_type> __il, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        map(__il.begin(), __il.end(), __comp, __a)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    map(_InputIterator __first, _InputIterator __last, const _Allocator& __a) :
        map(__first, __last, _Compare(), __a)
    {
    }

    map(initializer_list<value_type> __il, const _Allocator& __a) :
        map(__il, _Compare(), __a)
    {
    }

    ~map()
    {
        clear();
    }

    map& operator=(const map& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;
        
        clear();

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
        {
            _M_allocator = __x._M_allocator;
        }

        _M_compare = __x._M_compare;
        insert(__x.begin(), __x.end());

        return *this;
    }

    map& operator=(map&& __x)
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

    map& operator=(initializer_list<value_type> __il)
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

    mapped_type& operator[](const key_type& __k)
    {
        return try_emplace(__k).first->second;
    }

    mapped_type& operator[](key_type&& __k)
    {
        return try_emplace(std::move(__k)).first->second;
    }

    mapped_type& at(const key_type& __k)
    {
        auto __i = find(__k);
        if (__i == end())
            throw out_of_range("no such element in map");

        return __i->second;
    }
    
    const mapped_type& at(const key_type& __k) const
    {
        auto __i = find(__k);
        if (__i == end())
            throw out_of_range("no such element in map");

        return __i->second;
    }

    template <class... _Args>
    pair<iterator, bool> emplace(_Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        if (auto __i = _M_tree.__insert_unique(__n, __n->__data()->first, __compare()); __i != __n)
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
        if (auto __i = _M_tree.__insert_unique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare()); __i != __n)
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return __make_iter(__i);
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

    template <class _P>
        requires is_constructible_v<value_type, _P&&>
    pair<iterator, bool> insert(_P&& __p)
    {
        return emplace(std::forward<_P>(__p));
    }

    iterator insert(const_iterator __hint, const value_type& __x)
    {
        return emplace_hint(__hint, __x);
    }

    iterator insert(const_iterator __hint, value_type&& __x)
    {
        return emplace_hint(__hint, std::move(__x));
    }

    template <class _P>
        requires is_constructible_v<value_type, _P&&>
    iterator insert(const_iterator __hint, _P&& __p)
    {
        return emplace_hint(__hint, std::forward<_P>(__p));
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
        auto __i = _M_tree.__insert_unique(__n, __n->__data()->first, __compare());
        if (__i != __n)
            return {__make_iter(__i), false, std::move(__nh)};

        __nh._M_ptr = nullptr;
        return {__make_iter(__i), true, {}};
    }

    iterator insert(const_iterator __hint, node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_unique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare());
        if (__i != __n)
            return __make_iter(__i);

        __nh._M_ptr = nullptr;
        return __make_iter(__n);
    }

    template <class... _Args>
    pair<iterator, bool> try_emplace(const key_type& __k, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, value_type(std::piecewise_construct, std::forward_as_tuple(__k), std::forward_as_tuple(std::forward<_Args>(__args)...)));
        if (!_M_tree.__insert_unique(__n, __n->__data()->first, __compare()))
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return {end(), false};
        }

        return {__make_iter(__n), true};
    }

    template <class... _Args>
    pair<iterator, bool> try_emplace(key_type&& __k, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, value_type(std::piecewise_construct, std::forward_as_tuple(std::move(__k)), std::forward_as_tuple(std::forward<_Args>(__args)...)));
        if (!_M_tree.__insert_unique(__n, __n->__data()->first, __compare()))
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return {end(), false};
        }

        return {__make_iter(__n), true};
    }

    template <class... _Args>
    iterator try_emplace(const_iterator __hint, const key_type& __k, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, value_type(std::piecewise_construct, std::forward_as_tuple(__k), std::forward_as_tuple(std::forward<_Args>(__args)...)));
        if (!_M_tree.__insert_unique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare()))
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return {end(), false};
        }

        return {__make_iter(__n), true};
    }

    template <class... _Args>
    iterator try_emplace(const_iterator __hint, key_type&& __k, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, value_type(std::piecewise_construct, std::forward_as_tuple(std::move(__k)), std::forward_as_tuple(std::forward<_Args>(__args)...)));
        if (!_M_tree.__insert_unique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare()))
        {
            __rbtree::__delete_node(_M_allocator, __n);
            return {end(), false};
        }

        return {__make_iter(__n), true};
    }

    template <class _M>
        requires is_assignable_v<mapped_type&, _M&&>
    pair<iterator, bool> insert_or_assign(const key_type& __k, _M&& __obj)
    {
        auto [__i, __where] = _M_tree.__find_insert_point(__k, __compare());
        if (__where != __rbtree::__node::__where::__here)
            return {insert_hint(__make_iter(__i), value_type(__k, std::forward<_M>(__obj))), true};

        __i->__data()->second = std::forward<_M>(__obj);
        return {__make_iter(__i), false};
    }

    template <class _M>
        requires is_assignable_v<mapped_type&, _M&&>
    pair<iterator, bool> insert_or_assign(key_type&& __k, _M&& __obj)
    {
        auto [__i, __where] = _M_tree.__find_insert_point(__k, __compare());
        if (__where != __rbtree::__node::__where::__here)
            return {insert_hint(__make_iter(__i), value_type(std::move(__k), std::forward<_M>(__obj))), true};

        __i->__data()->second = std::forward<_M>(__obj);
        return {__make_iter(__i), false};
    }

    template <class _M>
        requires is_assignable_v<mapped_type&, _M&&>
    iterator insert_or_assign(const_iterator __hint, const key_type& __k, _M&& __obj)
    {
        auto [__i, __where] = _M_tree.__find_insert_point_hint(__hint.__ptr(), __k, __compare());
        if (__where != __rbtree::__node::__where::__here)
            return insert_hint(__make_iter(__i), value_type(__k, std::forward<_M>(__obj)));

        __i->__data()->second = std::forward<_M>(__obj);
        return __make_iter(__i);
    }

    template <class _M>
        requires is_assignable_v<mapped_type&, _M&&>
    iterator insert_or_assign(const_iterator __hint, key_type& __k, _M&& __obj)
    {
        auto [__i, __where] = _M_tree.__find_insert_point_hint(__hint.__ptr(), __k, __compare());
        if (__where != __rbtree::__node::__where::__here)
            return insert_hint(__make_iter(__i), value_type(std::move(__k), std::forward<_M>(__obj)));

        __i->__data()->second = std::forward<_M>(__obj);
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

    void swap(map& __x)
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
    void merge(map<_Key, _T, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            auto [__i, __where] = _M_tree.__find_insert_point(__b->first, __compare());
            if (__where != __rbtree::__node::__where::__here)
                insert(__make_iter(__i), __source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(map<_Key, _T, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    template <class _C2>
    void merge(multimap<_Key, _T, _C2, _Allocator>& __source)
    {
        auto __b = __source.begin();
        auto __e = __source.end();

        while (__b != __e)
        {
            auto __next = std::next(__b);
            auto [__i, __where] = _M_tree.__find_insert_point(__b->first, __compare());
            if (__where != __rbtree::__node::__where::__here)
                insert(__make_iter(__i), __source.extract(__b));

            __b = __next;
        }
    }

    template <class _C2>
    void merge(multimap<_Key, _T, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    key_compare key_comp() const
    {
        return _M_compare;
    }

    value_compare value_comp() const
    {
        return value_compare(key_comp());
    }

    iterator find(const key_type& __x)
    {
        return __make_iter(_M_tree.__find(__x, __compare()));
    }

    const_iterator find(const key_type& __x) const
    {
        return __make_iter(_M_tree.__find(__x, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator find(const _K& __k)
    {
        return __make_iter(_M_tree.__find(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator find(const _K& __k) const
    {
        return __make_iter(_M_tree.__find(__k, __compare()));
    }

    size_type count(const key_type& __x) const
    {
        auto [__b, __e] = equal_range(__x);
        return std::distance(__b, __e);
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    size_type count(const _K& __k) const
    {
        auto [__b, __e] = equal_range(__k);
        return std::distance(__b, __e);
    }

    bool contains(const key_type& __x) const
    {
        return find(__x) != end();
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    bool contains(const _K& __k) const
    {
        return find(__k) != end();
    }

    iterator lower_bound(const key_type& __x)
    {
        return __make_iter(_M_tree.__lower_bound(__x, __compare()));
    }

    const_iterator lower_bound(const key_type& __x) const
    {
        return __make_iter(_M_tree.__lower_bound(__x, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator lower_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator lower_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__lower_bound(__k, __compare()));
    }

    iterator upper_bound(const key_type& __x)
    {
        return __make_iter(_M_tree.__upper_bound(__x, __compare()));
    }

    const_iterator upper_bound(const key_type& __x) const
    {
        return __make_iter(_M_tree.__upper_bound(__x, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator upper_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator upper_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }

    pair<iterator, iterator> equal_range(const key_type& __x)
    {
        auto [__b, __e] = _M_tree.__equal_range(__x, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& __x) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__x, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<iterator, iterator> equal_range(const _K& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<const_iterator, const_iterator> equal_range(const _K& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
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

    constexpr auto __compare() const
    {
        return [this]<class _U, class _V>(const _U& __x, const _V& __y)
        {
            if constexpr (is_same_v<_U, value_type>)
            {
                if constexpr (is_same_v<_V, value_type>)
                    return _M_compare(__x.first, __y.first);
                else
                    return _M_compare(__x.first, __y);
            }
            else if constexpr (is_same_v<_V, value_type>)
                return _M_compare(__x, __y.first);
            else
                return _M_compare(__x, __y);
        };
    }
};

template <class _InputIterator, class _Compare = less<__detail::__iter_key_type<_InputIterator>>, class _Allocator = allocator<__detail::__iter_to_alloc_type<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
map(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
    -> map<__detail::__iter_key_type<_InputIterator>, __detail::__iter_mapped_type<_InputIterator>, _Compare, _Allocator>;

template <class _Key, class _T, class _Compare = less<_Key>, class _Allocator = allocator<pair<const _Key, _T>>>
    requires __detail::__is_allocator<_Allocator>
map(initializer_list<pair<_Key, _T>>, _Compare = _Compare(), _Allocator = _Allocator())
    -> map<_Key, _T, _Compare, _Allocator>;

template <class _InputIterator, class _Allocator>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
map(_InputIterator, _InputIterator, _Allocator)
    -> map<__detail::__iter_key_type<_InputIterator>, __detail::__iter_mapped_type<_InputIterator>, less<__detail::__iter_key_type<_InputIterator>>, _Allocator>;

template <class _Key, class _T, class _Allocator>
    requires __detail::__is_allocator<_Allocator>
map(initializer_list<pair<_Key, _T>>, _Allocator)
    -> map<_Key, _T, less<_Key>, _Allocator>;


template <class _Key, class _T, class _Compare, class _Allocator>
bool operator==(const map<_Key, _T, _Compare, _Allocator>& __x, const map<_Key, _T, _Compare, _Allocator>& __y)
{
    if (__x.size() != __y.size())
        return false;

    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _T, class _Compare, class _Allocator>
__detail::__synth_three_way_result<pair<const _Key, _T>> operator<=>(const map<_Key, _T, _Compare, _Allocator>& __x, const map<_Key, _T, _Compare, _Allocator>& __y)
{
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}

template <class _Key, class _T, class _Compare, class _Allocator>
void swap(map<_Key, _T, _Compare, _Allocator>& __x, map<_Key, _T, _Compare, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Key, class _T, class _Compare, class _Allocator, class _Predicate>
typename map<_Key, _T, _Compare, _Allocator>::size_type erase_if(map<_Key, _T, _Compare, _Allocator>& __c, _Predicate __pred)
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


template <class _Key, class _T, class _Compare, class _Allocator>
class multimap
{
public:

    class value_compare;

    using __rbtree = __detail::__rbtree<pair<const _Key, _T>>;

    using key_type                  = _Key;
    using mapped_type               = _T;
    using value_type                = pair<const _Key, _T>;
    using key_compare               = _Compare;
    using allocator_type            = _Allocator;
    using pointer                   = typename allocator_traits<_Allocator>::pointer;
    using const_pointer             = typename allocator_traits<_Allocator>::const_pointer;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = typename __rbtree::template __iterator<false>;
    using const_iterator            = typename __rbtree::template __iterator<true>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using node_type                 = __detail::__map_node_handle<_Key, _T, _Allocator>;
    using insert_return_type        = __detail::__insert_return_type<iterator, node_type>;

    class value_compare
    {
        friend class multimap;

    protected:

        _Compare _M_comp;

        value_compare(_Compare __c) :
            _M_comp(__c)
        {
        }

    public:

        bool operator()(const value_type& __x, const value_type& __y) const
        {
            return _M_comp(__x.first, __y.first);
        }
    };


    multimap() :
        multimap(_Compare())
    {
    }

    explicit multimap(const _Compare& __comp, const _Allocator& __a = _Allocator()) :
        _M_allocator(__a), _M_compare(__comp)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    multimap(_InputIterator __first, _InputIterator __last, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        multimap(__comp, __a)
    {
        insert(__first, __last);
    }

    multimap(const multimap& __x) :
        _M_allocator(__x._M_allocator),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    multimap(multimap&& __x) :
        _M_allocator(std::move(__x._M_allocator)),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    explicit multimap(const _Allocator& __a) :
        _M_allocator(__a)
    {
    }

    multimap(const multimap& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(__x._M_compare)
    {
        insert(__x.begin(), __x.end());
    }

    multimap(multimap&& __x, const _Allocator& __a) :
        _M_allocator(__a),
        _M_compare(std::move(__x._M_compare)),
        _M_tree(std::move(__x._M_tree))
    {
    }

    multimap(initializer_list<value_type> __il, const _Compare& __comp = _Compare(), const _Allocator& __a = _Allocator()) :
        multimap(__il.begin(), __il.end(), __comp, __a)
    {
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    multimap(_InputIterator __first, _InputIterator __last, const _Allocator& __a) :
        multimap(__first, __last, _Compare(), __a)
    {
    }

    multimap(initializer_list<value_type> __il, const _Allocator& __a) :
        multimap(__il, _Compare(), __a)
    {
    }

    ~multimap()
    {
        clear();
    }

    multimap& operator=(const multimap& __x)
    {
        if (&__x == this) [[unlikely]]
            return *this;

        clear();

        if constexpr (allocator_traits<_Allocator>::propagate_on_container_copy_assignment::value
                      && !allocator_traits<_Allocator>::is_always_equal::value)
        {
            _M_allocator = __x._M_allocator;
        }

        _M_compare = __x._M_compare;
        insert(__x.begin(), __x.end());

        return *this;
    }

    multimap& operator=(multimap&& __x)
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

    multimap& operator=(initializer_list<value_type> __il)
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
        return __make_iter(_M_tree.__insert_nonunique(__n, __n->__data()->first, __compare()));
    }

    template <class... _Args>
    iterator emplace_hint(const_iterator __hint, _Args&&... __args)
    {
        auto __n = __rbtree::__new_node(_M_allocator, std::forward<_Args>(__args)...);
        return __make_iter(_M_tree.__insert_nonunique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare()));
    }

    iterator insert(const value_type& __x)
    {
        return emplace(__x);
    }

    iterator insert(value_type&& __x)
    {
        return emplace(std::move(__x));
    }

    template <class _P>
        requires is_constructible_v<value_type, _P&&>
    iterator insert(_P&& __p)
    {
        return emplace(std::forward<_P>(__p));
    }

    iterator insert(const_iterator __hint, const value_type& __x)
    {
        return emplace_hint(__hint, __x);
    }

    iterator insert(const_iterator __hint, value_type&& __x)
    {
        return emplace_hint(__hint, std::move(__x));
    }

    template <class _P>
        requires is_constructible_v<value_type, _P&&>
    iterator insert(const_iterator __hint, _P&& __p)
    {
        return emplace_hint(__hint, std::forward<_P>(__p));
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
        if (__i == end() || __compare()(__k, __i->first))
            return {};

        return extract(__i);
    }

    iterator insert(node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_nonunique(__n, __n->__data()->first, __compare());

        __nh._M_ptr = nullptr;
        return __make_iter(__i);
    }

    iterator insert(const_iterator __hint, node_type&& __nh)
    {
        if (!__nh)
            return end();

        auto __n = __nh._M_ptr;
        auto __i = _M_tree.__insert_nonunique_hint(__hint.__ptr(), __n, __n->__data()->first, __compare());
        
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

    void swap(multimap& __x)
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
    void merge(multimap<_Key, _T, _C2, _Allocator>& __source)
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
    void merge(multimap<_Key, _T, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    template <class _C2>
    void merge(map<_Key, _T, _C2, _Allocator>& __source)
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
    void merge(map<_Key, _T, _C2, _Allocator>&& __source)
    {
        merge(__source);
    }

    key_compare key_comp() const
    {
        return _M_compare;
    }

    value_compare value_comp() const
    {
        return value_compare(key_comp());
    }

    iterator find(const key_type& __x)
    {
        return __make_iter(_M_tree.__find(__x, __compare()));
    }

    const_iterator find(const key_type& __x) const
    {
        return __make_iter(_M_tree.__find(__x, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator find(const _K& __k)
    {
        return __make_iter(_M_tree.find(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator find(const _K& __k) const
    {
        return __make_iter(_M_tree.find(__k, __compare()));
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

    iterator lower_bound(const key_type& __x)
    {
        return __make_iter(_M_tree.__lower_bound(__x, __compare()));
    }

    const_iterator lower_bound(const key_type& __x) const
    {
        return __make_iter(_M_tree.__lower_bound(__x, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator lower_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__lower_bound(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator lower_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__lower_bound(__k, __compare()));
    }

    iterator upper_bound(const key_type& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }
    
    const_iterator upper_bound(const key_type& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    iterator upper_bound(const _K& __k)
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    const_iterator upper_bound(const _K& __k) const
    {
        return __make_iter(_M_tree.__upper_bound(__k, __compare()));
    }

    pair<iterator, iterator> equal_range(const key_type& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<iterator, iterator> equal_range(const _K& __k)
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
        return {__make_iter(__b), __make_iter(__e)};
    }

    template <class _K>
        requires __detail::__transparent_compare<_Compare>
    pair<const_iterator, const_iterator> equal_range(const _K& __k) const
    {
        auto [__b, __e] = _M_tree.__equal_range(__k, __compare());
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

    constexpr auto __compare() const
    {
        return [this]<class _U, class _V>(const _U& __x, const _V& __y)
        {
            if constexpr (is_same_v<_U, value_type>)
            {
                if constexpr (is_same_v<_V, value_type>)
                    return _M_compare(__x.first, __y.first);
                else
                    return _M_compare(__x.first, __y);
            }
            else if constexpr (is_same_v<_V, value_type>)
                return _M_compare(__x, __y.first);
            else
                return _M_compare(__x, __y);
        };
    }
};

template <class _InputIterator, class _Compare = less<__detail::__iter_key_type<_InputIterator>>, class _Allocator = allocator<__detail::__iter_to_alloc_type<_InputIterator>>>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
multimap(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
    -> multimap<__detail::__iter_key_type<_InputIterator>, __detail::__iter_mapped_type<_InputIterator>, _Compare, _Allocator>;

template <class _Key, class _T, class _Compare = less<_Key>, class _Allocator = allocator<pair<const _Key, _T>>>
    requires __detail::__is_allocator<_Allocator>
multimap(initializer_list<pair<_Key, _T>>, _Compare = _Compare(), _Allocator = _Allocator())
    -> multimap<_Key, _T, _Compare, _Allocator>;

template <class _InputIterator, class _Allocator>
    requires __detail::__cpp17_input_iterator<_InputIterator> && __detail::__is_allocator<_Allocator>
multimap(_InputIterator, _InputIterator, _Allocator)
    -> multimap<__detail::__iter_key_type<_InputIterator>, __detail::__iter_mapped_type<_InputIterator>, less<__detail::__iter_key_type<_InputIterator>>, _Allocator>;

template <class _Key, class _T, class _Allocator>
    requires __detail::__is_allocator<_Allocator>
multimap(initializer_list<pair<_Key, _T>>, _Allocator)
    -> multimap<_Key, _T, less<_Key>, _Allocator>;


template <class _Key, class _T, class _Compare, class _Allocator>
bool operator==(const multimap<_Key, _T, _Compare, _Allocator>& __x, const multimap<_Key, _T, _Compare, _Allocator>& __y)
{
    if (__x.size() != __y.size())
        return false;

    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _T, class _Compare, class _Allocator>
__detail::__synth_three_way_result<pair<const _Key, _T>> operator<=>(const multimap<_Key, _T, _Compare, _Allocator>& __x, const multimap<_Key, _T, _Compare, _Allocator>& __y)
{
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}

template <class _Key, class _T, class _Compare, class _Allocator>
void swap(multimap<_Key, _T, _Compare, _Allocator>& __x, multimap<_Key, _T, _Compare, _Allocator>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Key, class _T, class _Compare, class _Allocator, class _Predicate>
typename multimap<_Key, _T, _Compare, _Allocator>::size_type erase_if(multimap<_Key, _T, _Compare, _Allocator>& __c, _Predicate __pred)
{
    auto __orig_size = __c.size();
    for (auto __i = __c.begin(), __last = __c.end(); __i != __last; )
    {
        if (__pred(*__i))
            __i = __c.erase(__i);
        else
            ++__i;
    }

    return __orig_size - __c.size();
}


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_MAP_H */

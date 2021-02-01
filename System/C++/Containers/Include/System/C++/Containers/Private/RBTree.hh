#ifndef __SYSTEM_CXX_CONTAINERS_PRIVATE_RBTREE_H
#define __SYSTEM_CXX_CONTAINERS_PRIVATE_RBTREE_H


#include <System/C++/Containers/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/Bit.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace __XVI_STD_CONTAINERS_NS::__detail
{


template <class _InputIterator>
using __iter_key_type = remove_const_t<typename iterator_traits<_InputIterator>::value_type::first_type>;

template <class _InputIterator>
using __iter_mapped_type = typename iterator_traits<_InputIterator>::value_type::second_type;

template <class _InputIterator>
using __iter_value_type = typename iterator_traits<_InputIterator>::value_type;

template <class _InputIterator>
using __iter_to_alloc_type = pair<add_const_t<typename iterator_traits<_InputIterator>::value_type::first_type>, typename iterator_traits<_InputIterator>::value_type::second_type>;


template <class _Compare>
concept __transparent_compare = requires { typename _Compare::is_transparent; };


// Red-black tree.
template <class _Value>
class __rbtree
{
public:

    class __node
    {
    public:

        friend class __rbtree;

        struct __node_retval
        {
            __node*     __new_root;
            __node*     __affected_node;
        };

        enum class __where
        {
            __left      = -1,
            __here      = 0,
            __right     = 1,
        };


        __node() = delete;
        __node(const __node&) = delete;
        
        ~__node() = default;


        constexpr __node* __left() const noexcept
        {
            return std::bit_cast<__node*>(_M_left);
        }

        constexpr __node* __right() const noexcept
        {
            return std::bit_cast<__node*>(_M_right);
        }

        constexpr __node* __parent() const noexcept
        {
            return std::bit_cast<__node*>(_M_parent & __ptr_mask);
        }

        constexpr _Value* __data() noexcept
        {
            return std::addressof(_M_value);
        }

        constexpr const _Value* __data() const noexcept
        {
            return std::addressof(_M_value);
        }


        static constexpr __node* __min(__node* __n) noexcept
        {
            while (__n->__left())
                __n = __n->__left();

            return __n;
        }

        static constexpr __node* __max(__node* __n) noexcept
        {
            while (__n->__right())
                __n = __n->__right();

            return __n;
        }

        static constexpr __node* __next(__node* __n) noexcept
        {
            if (__n->__right())
                return __min(__n->__right());
            else if (__n->__is_left_child())
                return __n->__parent();
            else if (__n->__is_right_child())
            {
                while (__n->__is_right_child())
                    __n = __n->__parent();

                return __n->__parent();
            }

            return nullptr;
        }

        static constexpr __node* __prev(__node* __n) noexcept
        {
            if (__n->__left())
                return __max(__n->__left());
            else if (__n->__is_right_child())
                return __n->__parent();
            else if (__n->__is_left_child())
            {
                while (__n->__is_left_child())
                    __n = __n->__parent();

                return __n->__parent();
            }

            return nullptr;
        }

        template <class _Key, class _Predicate>
        static constexpr pair<__node*, __where> __find_insert_point(__node* __root, _Key __key, _Predicate __pred)
        {
            if (!__root)
                return {nullptr, __where::__here};

            if (__pred(__key, __root->_M_value))
            {
                if (__root->__left())
                    return __find_insert_point(__root, __key, __pred);
                return {__root, __where::__left};
            }
            else if (__pred(__root->_M_value, __key))
            {
                if (__root->__right())
                    return __find_insert_point(__root, __key, __pred);
                return {__root, __where::__right};
            }

            return {__root, __where::__here};
        }

        template <class _Key, class _Predicate>
        static constexpr pair<__node*, __where> __find_insert_point_hint(__node* __hint, __node* __root, _Key __key, _Predicate __pred)
        {
            if (__pred(__key, __hint->_M_value))
            {
                auto __p = __prev(__hint);
                if (__p && !__pred(__key, __p->_M_value))
                    return {__hint, __where::__left};
            }
            else if (__pred(__hint->_M_value, __key))
            {
                auto __n = __next(__hint);
                if (__n && !__pred(__n->_M_value, __key))
                    return {__hint, __where::__right};
            }
            else
            {
                // Equal keys.
                return {__hint, __where::__here};
            }

            return __find_insert_point(__root, __key, __pred);
        }

        template <class _Key, class _Predicate>
        static constexpr __node* __find(__node* __root, _Key __key, _Predicate __pred)
        {
            if (!__root)
                return nullptr;

            if (__pred(__key, __root->_M_value))
                return __find(__root->__left(), __key, __pred);
            else if (__pred(__root->_M_value, __key))
                return __find(__root->__right(), __key, __pred);
            else
                return __root;
        }

        template <class _Key, class _Predicate>
        static constexpr __node* __lower_bound(__node* __root, _Key __key, _Predicate __pred)
        {
            if (!__root)
                return nullptr;
            
            auto __at = __find_insert_point(__root, __key, __pred);
            if (__at.second == __where::__left)
                return __next(__at.first);
            else if (__at.second == __where::__right)
                return __at.first;

            __root = __at.first;

            // The lower bound is this node or is part of the left-most subtree.
            auto __l = __root->__left();
            while (__l)
            {
                if (__pred(__l->_M_value, __key))
                    __l = __l->__right();
                else
                {
                    __root = __l;
                    __l = __l->__left();
                }
            }

            return __root;
        }

        template <class _Key, class _Predicate>
        static constexpr __node* __upper_bound(__node* __root, _Key __key, _Predicate __pred)
        {
            if (!__root)
                return nullptr;
            
            auto __at = __find_insert_point(__root, __key, __pred);
            if (__at.second == __where::__left)
                return __at.first;
            else if (__at.second == __where::__right)
                return __next(__at.first);

            __root = __at.first;

            // The upper bound is this node or is part of the right-most subtree.
            auto __r = __root->__right();
            while (__r)
            {
                if (__pred(__key, __r->_M_value))
                    __r = __r->__left();
                else
                {
                    __root = __r;
                    __r = __r->__right();
                }
            }

            // Point to the first element greater than the key.
            return __next(__root);
        }

        template <class _Key, class _Predicate>
        static constexpr pair<__node*, __node*> __equal_range(__node* __root, _Key __key, _Predicate __pred)
        {
            if (!__root)
                return {nullptr, nullptr};
            
            auto __at = __find_insert_point(__root, __key, __pred);
            if (__at.second == __where::__left)
            {
                auto __n = __next(__at.first);
                return {__n, __n};
            }
            else if (__at.second == __where::__right)
            {
                return {__at.first, __at.first};
            }

            __root = __at.first;

            return {__lower_bound(__root, __key, __pred), __upper_bound(__root, __key, __pred)};
        }

        template <class _Key, class _Predicate>
        static constexpr __node* __insert(__node* __root, __node* __new, _Key __key, _Predicate __pred)
        {
            __insert_unbalanced(__root, __new, __key, __pred);
            __insert_balance(__new);

            while (__parent(__new))
                __new = __parent(__new);

            return __new;
        }

        static constexpr __node_retval __remove(__node* __del)
        {
            // If this is an internal node, find the in-order predecessor and switch places with it.
            if (__del->__right() && __del->__left())
            {
                // Always exists.
                auto __pre = __prev(__del);
                __exchange_nodes(__pre, __del);
            }

            // Unlink the node from the tree.
            auto __p = __parent(__del);
            auto __c = __del->__left() ? __del->__left() : __del->__right();
            __c->__set_parent(__p);
            if (__del->__is_left_child())
                __p->__set_left(__c);
            else
                __p->__set_right(__c);

            // Rebalance the tree.
            __delete_balance(__c);

            while (__parent(__c))
                __c = __parent(__c);

            return {__c, __del};
        }

        static constexpr __node_retval __remove_max(__node* __n) noexcept
        {
            if (!__n)
                return {nullptr, nullptr};

            while (__n->__right())
                __n = __n->__right();

            return __remove(__n);
        }

        static constexpr __node_retval __remove_min(__node* __n) noexcept
        {
            if (!__n)
                return {nullptr, nullptr};

            while (__n->__left())
                __n = __n->__left();

            return __remove(__n);
        }


    private:

        static constexpr std::uintptr_t     __flags_mask    = 0b11;
        static constexpr std::uintptr_t     __ptr_mask      = ~__flags_mask;
        static constexpr std::uintptr_t     __flag_red      = 0b01;

        std::uintptr_t  _M_left     = 0;
        std::uintptr_t  _M_right    = 0;
        std::uintptr_t  _M_parent   = __flag_red;

        _Value      _M_value        = {};


        constexpr void __set_left(__node* __l) noexcept
        {
            _M_left = std::bit_cast<std::uintptr_t>(__l);
            if (__l)
                __l->__set_parent(this);
        }

        constexpr void __set_right(__node* __r) noexcept
        {
            _M_right = std::bit_cast<std::uintptr_t>(__r);
            if (__r)
                __r->__set_parent(this);
        }

        constexpr void __set_parent(__node* __p) noexcept
        {
            _M_parent = std::bit_cast<std::uintptr_t>(__p) | (_M_parent & __flags_mask);
        }

        constexpr bool __is_red() const noexcept
        {
            return std::bit_cast<std::uintptr_t>(_M_parent) & __flag_red;
        }

        constexpr bool __is_black() const noexcept
        {
            return !__is_red();
        }

        constexpr void __set_red() noexcept
        {
            _M_left |= __flag_red;
        }

        constexpr void __set_black() noexcept
        {
            _M_left &= ~__flag_red;
        }

        constexpr void __set_red(bool __red) noexcept
        {
            if (__red)
                __set_red();
            else
                __set_black();
        }

        constexpr bool __is_left_child() const noexcept
        {
            return __parent() && __parent()->__left() == this;
        }

        constexpr bool __is_right_child() const noexcept
        {
            return __parent() && __parent()->__right() == this;
        }


        static constexpr __node* __parent(__node* __n) noexcept
        {
            return __n ? __n->__parent() : nullptr;
        }

        static constexpr __node* __grandparent(__node* __n) noexcept
        {
            return __parent(__parent(__n));
        }

        static constexpr __node* __sibling(__node* __n) noexcept
        {
            auto __p = __parent(__n);
            if (!__p)
                return nullptr;

            if (__p->__left() == __n)
                return __p->__right();
            else
                return __p->__left();
        }

        static constexpr __node* __uncle(__node* __n) noexcept
        {
            auto __p = __parent(__n);
            if (!__p)
                return nullptr;

            return __sibling(__p);
        }

        static constexpr bool __is_black(__node* __n) noexcept
        {
            return (!__n || __n->__is_black());
        }

        static constexpr bool __is_red(__node* __n) noexcept
        {
            return !__is_black(__n);
        }

        static constexpr __node* __rotate_right(__node* __n) noexcept
        {
            auto* __x = __n->__right();
            __x->__set_parent(__n->__parent());
            __n->__set_right(__x->__left());
            __x->__set_left(__n);
            __x->__set_red(__n->__is_red());
            __n->__set_red();

            return __x;
        }

        static constexpr __node* __rotate_left(__node* __n) noexcept
        {
            auto* __x = __n->__left();
            __x->__set_parent(__n->__parent());
            __n->__set_left(__x->__right());
            __x->__set_right(__n);
            __x->__set_red(__n->__is_red());
            __n->__set_red();

            return __x;
        }

        static void __exchange_nodes(__node* __old, __node* __new)
        {
            using std::swap;

            bool __old_was_left = __old->__is_left_child();
            bool __new_was_left = __new->__is_left_child();

            swap(__old->_M_left, __new->_M_left);
            swap(__old->_M_right, __new->_M_right);
            swap(__old->_M_parent, __new->_M_parent);

            if (__old_was_left)
                __new->__parent()->__set_left(__new);
            else
                __new->__parent()->__set_right(__new);

            if (__new_was_left)
                __old->__parent()->__set_left(__old);
            else
                __old->__parent()->__set_right(__old);
        }

        template <class _Key, class _Predicate>
        static constexpr void __insert_unbalanced(__node* __root, __node* __new, _Key __key, _Predicate __pred)
        {
            auto* __l = __root->__left();
            auto* __r = __root->__right();

            if (__root)
            {
                if (__pred(__key, __root->_M_value))
                {
                    if (__l)
                    {
                        __insert_unbalanced(__l, __new, __key, __pred);
                        return;
                    }

                    __root->__set_left(__new);
                }
                else
                {
                    if (__r)
                    {
                        __insert_unbalanced(__r, __new, __key, __pred);
                        return;
                    }

                    __root->__set_right(__new);
                }
            }

            __new->__set_parent(__root);
            __new->__set_right(nullptr);
            __new->__set_left(nullptr);
            __new->__set_red();
        }

        static constexpr void __insert_balance(__node* __n) noexcept
        {
            // The input node is coloured red initially.

            if (!__parent(__n))
            {
                // We'e at the root; re-colour it to black.
                __n->__set_black();
            }
            else if (__is_black(__parent(__n)))
            {
                // No violation of tree invariants; nothing to do.
            }
            else if (__is_red(__uncle(__n)))
            {
                // We have two reds in a row and both the parent and uncle are red. Recolour them and the grandparent
                // and then continue balancing further up the tree.
                __parent(__n)->__set_black();
                __uncle(__n)->__set_black();
                __grandparent(__n)->__set_red();
                __insert_balance(__grandparent(__n));
            }
            else
            {
                // We have two reds in a row but only the parent is red and not the uncle so we'll need to rotate the
                // tree before recolouring.
                auto* __p = __parent(__n);
                auto* __g = __parent(__p);

                if (__n->__is_right_child() && __p->__is_left_child())
                {
                    __rotate_left(__p);
                    __n = __n->__left();
                }
                else if (__n->__is_left_child() && __p->__is_right_child())
                {
                    __rotate_right(__p);
                    __n = __n->__right();
                }

                __p = __parent(__n);
                __g = __parent(__p);

                if (__n->__is_left_child())
                    __rotate_right(__g);
                else
                    __rotate_left(__g);

                __p->__set_black();
                __g->__set_red();
            }
        }

        static constexpr void __delete_balance(__node* __n) noexcept
        {
            if (!__parent(__n))
            {
                // We're at the root; nothing needs to be done.
                return;
            }

            auto __s = __sibling(__n);
            
            if (__is_red(__s))
            {
                __n->__parent()->__set_red();
                __s->__set_black();
                if (__n->__is_left_child())
                    __rotate_left(__n->__parent());
                else
                    __rotate_right(__n->__parent());
            }

            __s = __sibling(__n);

            if (__n->__parent()->__is_black()
                && __s->__is_black()
                && __s->__left()->__is_black()
                && __s->__right()->__is_black())
            {
                __s->__set_red();
                __delete_balance(__n->__parent());
                return;
            }
            
            if (__n->__parent()->__is_red()
                && __s->__is_black()
                && __s->__left()->__is_black()
                && __s->__right()->__is_black())
            {
                __s->__set_red();
                __n->__parent()->__set_black();
                return;
            }

            if (__n->__is_left_child() && __s->__right()->__is_black() && __s->__left()->__is_red())
            {
                __s->__set_red();
                __s->__left()->__set_black();
                __rotate_right(__s);
            }
            else if (__n->__is_right_child() && __s->__left()->__is_black() && __s->__right()->__is_red())
            {
                __s->__set_red();
                __s->__right()->__set_black();
                __rotate_left(__s);
            }

            __s = __sibling(__n);

            __s->__set_red(__n->__parent()->__is_red());
            __n->__parent()->__set_black();

            if (__n->__is_left_child())
            {
                __s->__right()->__set_black();
                __rotate_left(__n->__parent());
            }
            else
            {
                __s->__left()->__set_black();
                __rotate_right(__n->__parent());
            }
        }
    };

    template <bool _Const, bool _ReadOnly = false>
    class __iterator
    {
    public:

        using value_type        = _Value;
        using pointer           = _Value*;
        using reference         = _Value&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = bidirectional_iterator_tag;

        constexpr __iterator() = default;
        constexpr __iterator(const __iterator&) = default;

        template <bool _C, bool _RO>
        constexpr __iterator(const __iterator<_C, _RO>& __x)
            requires (_Const >= _C && _ReadOnly >= _RO) :
            _M_ptr(__x._M_ptr)
        {
        }

        constexpr __iterator& operator=(const __iterator&) = default;

        template <bool _C, bool _RO>
        constexpr __iterator& operator=(const __iterator<_C, _RO>& __x)
            requires (_Const >= _C && _ReadOnly >= _RO)
        {
            _M_ptr = __x._M_ptr;
            return *this;
        }

        constexpr ~__iterator() = default;

        constexpr _Value& operator*() const
            requires (!_Const && !_ReadOnly)
        {
            return *_M_ptr->__data();
        }

        constexpr const _Value& operator*() const
            requires (_Const || _ReadOnly)
        {
            return *_M_ptr->__data();
        }

        constexpr _Value* operator->() const
            requires (!_Const && !_ReadOnly)
        {
            return _M_ptr->__data();
        }

        constexpr const _Value* operator->() const
            requires (_Const || _ReadOnly)
        {
            return _M_ptr->__data();
        }

        constexpr __iterator& operator++()
        {
            _M_ptr = __node::__next(_M_ptr);
            return *this;
        }

        constexpr __iterator operator++(int)
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
        {
            _M_ptr = __node::__prev(_M_ptr);
            return *this;
        }

        constexpr __iterator operator--(int)
        {
            auto __tmp = *this;
            --__tmp;
            return __tmp;
        }

        constexpr bool operator==(const __iterator&) const = default;

        constexpr __node* __ptr() const
        {
            return _M_ptr;
        }

    private:

        friend class __rbtree;

        __node*     _M_ptr = nullptr;

        constexpr explicit __iterator(__node* __ptr) :
            _M_ptr(__ptr)
        {
        }
    };


    template <class _Allocator>
    using __node_alloc = typename allocator_traits<_Allocator>::template rebind_traits<__node>;


    constexpr __rbtree() = default;

    __rbtree(const __rbtree&) = delete;

    constexpr __rbtree(__rbtree&& __x) :
        _M_root(__x._M_root),
        _M_min(__x._M_min),
        _M_max(__x._M_max),
        _M_size(__x._M_size)
    {
        __x._M_root = nullptr;
        __x._M_min = nullptr;
        __x._M_max = nullptr;
        __x._M_size = 0;
    }

    __rbtree& operator=(const __rbtree&) = delete;

    constexpr __rbtree& operator=(__rbtree&& __x)
    {
        swap(_M_root, __x._M_root);
        swap(_M_min, __x._M_min);
        swap(_M_max, __x._M_max);
        swap(_M_size, __x._M_size);
        return *this;
    }

    constexpr ~__rbtree()
    {
    }


    constexpr __iterator<false> __begin()
    {
        return __iterator<false>(_M_min);
    }

    constexpr __iterator<true> __begin() const
    {
        return __iterator<true>(_M_min);
    }

    constexpr __iterator<false> __end()
    {
        return ++__iterator<false>(_M_max);
    }

    constexpr __iterator<true> __end() const
    {
        return ++__iterator<true>(_M_max);
    }

    constexpr std::size_t __size() const noexcept
    {
        return _M_size;
    }

    constexpr void swap(__rbtree& __x) noexcept
    {
        using std::swap;
        swap(_M_root, __x._M_root);
        swap(_M_min, __x._M_min);
        swap(_M_max, __x._M_max);
        swap(_M_size, __x._M_size);
    }

    template <class _Key, class _Compare>
    constexpr __node* __find(const _Key& __k, const _Compare& __comp) const
    {
        return __node::__find(_M_root, __k, __comp);
    }

    template <class _Key, class _Compare>
    constexpr __node* __lower_bound(const _Key& __k, const _Compare& __comp) const
    {
        return __node::__lower_bound(_M_root, __k, __comp);
    }

    template <class _Key, class _Compare>
    constexpr __node* __upper_bound(const _Key& __k, const _Compare& __comp) const
    {
        return __node::__upper_bound(_M_root, __k, __comp);
    }

    template <class _Key, class _Compare>
    constexpr pair<__node*, __node*> __equal_range(const _Key& __k, const _Compare& __comp) const
    {
        return __node::__equal_range(_M_root, __k, __comp);
    }

    constexpr __iterator<false> __make_iter(__node* __n) const
    {
        return __iterator<false>(__n);
    }

    template <class _Key, class _Predicate>
    __node* __insert_unique(__node* __new, _Key __key, _Predicate __pred)
    {
        if (!_M_root)
        {
            _M_root = _M_min = _M_max = __new;
            return __new;
        }
        
        auto __at = __node::__find_insert_point(_M_root, __key, __pred);
        if (__at.second == __node::__where::__here)
            return __at.first;

        _M_root = __node::__insert(__at.first, __new, __key, __pred);
        _M_size++;
        __update_minmax_after_insert(__new, __key, __pred);
        return __new;
    }

    template <class _Key, class _Predicate>
    __node* __insert_unique_hint(__node* __hint, __node* __new, _Key __key, _Predicate __pred)
    {
        if (!_M_root)
        {
            _M_root = _M_min = _M_max = __new;
            return __new;
        }
        
        auto __at = __node::__find_insert_point_hint(__hint, _M_root, __key, __pred);
        if (__at.second == __node::__where::__here)
            return __at.first;

        _M_root = __node::__insert(__at.first, __new, __key, __pred);
        _M_size++;
        __update_minmax_after_insert(__new, __key, __pred);
        return __new;
    }

    template <class _Key, class _Predicate>
    __node* __insert_nonunique(__node* __new, _Key __key, _Predicate __pred)
    {
        if (!_M_root)
        {
            _M_root = _M_min = _M_max = __new;
            return __new;
        }
        
        auto __at = __node::__find_insert_point(_M_root, __key, __pred);
        if (__at.second == __node::__where::__here)
        {
            __at.first = __node::__upper_bound(__at.first, __key, __pred);
            __at.second = __node::__where::__left;
            if (!__at.first)
            {
                __at.first = _M_max;
                __at.second = __node::__where::__right;
            }
        }

        _M_root = __node::__insert(__at.first, __new, __key, __pred);
        _M_size++;
        __update_minmax_after_insert(__new, __key, __pred);
        return __new;
    }

    template <class _Key, class _Predicate>
    __node* __insert_nonunique_hint(__node* __hint, __node* __new, _Key __key, _Predicate __pred)
    {
        if (!_M_root)
        {
            _M_root = _M_min = _M_max = __new;
            return __new;
        }
        
        auto __at = __node::__find_insert_point_hint(__hint, _M_root, __key, __pred);
        if (__at.second == __node::__where::__here)
        {
            __at.first = __node::__upper_bound(__at.first, __key, __pred);
            __at.second = __node::__where::__left;
            if (!__at.first)
            {
                __at.first = _M_max;
                __at.second = __node::__where::__right;
            }
        }

        _M_root = __node::__insert(__at.first, __new, __key, __pred);
        _M_size++;
        __update_minmax_after_insert(__new, __key, __pred);
        return __new;
    }

    template <class _Key, class _Predicate>
    auto __find_insert_point(_Key __key, _Predicate __pred)
    {
        return __node::__find_insert_point(_M_root, __key, __pred);
    }

    template <class _Key, class _Predicate>
    auto __find_insert_point_hint(__node* __hint, _Key __key, _Predicate __pred)
    {
        return __node::__find_insert_point_hint(__hint, _M_root, __key, __pred);
    }

    constexpr void __extract(__node* __which)
    {
        auto [__root, __n] = __node::__remove(__which);
        _M_root = __root;
        _M_size--;
        __update_minmax_after_removal(__n);
    }


    template <class _Allocator>
    static constexpr __node* __allocate_node(_Allocator& __a)
    {
        return __node_alloc<_Allocator>::allocate(__a, 1);
    }

    template <class _Allocator>
    static constexpr void __deallocate_node(_Allocator& __a, __node* __n)
    {
        __node_alloc<_Allocator>::deallocate(__a, __n, 1);
    }

    template <class _Allocator>
    static constexpr void __delete_node(_Allocator& __a, __node* __n) noexcept
    {
        __node_alloc<_Allocator>::destroy(__a, __n);
        __deallocate_node(__a, __n);
    }

    template <class _Allocator, class... _Args>
    static constexpr __node* __new_node(_Allocator& __a, _Args&&... __args)
    {
        auto* __n = __allocate_node(__a);
        if (!__n)
            throw bad_alloc();

        try
        {
            new (&__n->_M_left) std::uintptr_t(0);
            new (&__n->_M_right) std::uintptr_t(0);
            new (&__n->_M_parent) std::uintptr_t(0);

            allocator_traits<_Allocator>::construct(__a, &__n->_M_value, std::forward<_Args>(__args)...);

            __n->__set_red();

            return __n;
        }
        catch (...)
        {
            __deallocate_node(__a, __n);
            throw;
        }
    }


private:

    __node*     _M_root     = nullptr;
    __node*     _M_min      = nullptr;
    __node*     _M_max      = nullptr;
    std::size_t _M_size     = 0;


    template <class _Key, class _Predicate>
    constexpr void __update_minmax_after_insert(__node* __n, _Key __key, _Predicate __pred)
    {
        if (!_M_min || __pred(__key, _M_min->_M_value))
            _M_min = __n;

        if (_M_max || __pred(_M_max->_M_value, __key))
            _M_max = __n;
    }

    constexpr void __update_minmax_after_removal(__node* __n)
    {
        if (__n == _M_min)
            _M_min = __node::__min(_M_root);
        else if (__n == _M_max)
            _M_max = __node::__max(_M_root);
    }
};


template <class _Value>
constexpr void swap(__rbtree<_Value>& __x, __rbtree<_Value>& __y) noexcept
{
    __x.swap(__y);
}


template <class _Iterator, class _NodeType>
struct __insert_return_type
{
    _Iterator   position;
    bool        inserted;
    _NodeType   node;
};


} // namespace __XVI_STD_CONTAINERS_NS::__detail


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_PRIVATE_RBTREE_H */

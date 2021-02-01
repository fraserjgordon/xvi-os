#ifndef __SYSTEM_CXX_CONTAINERS_QUEUE_H
#define __SYSTEM_CXX_CONTAINERS_QUEUE_H


#include <System/C++/Containers/Private/Config.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Memory.hh>

#include <System/C++/Containers/Deque.hh>
#include <System/C++/Containers/Vector.hh>


namespace __XVI_STD_CONTAINERS_NS
{


template <class _T, class _Container = deque<_T>>
class queue
{
public:

    using value_type            = typename _Container::value_type;
    using reference             = typename _Container::reference;
    using const_reference       = typename _Container::const_reference;
    using size_type             = typename _Container::size_type;
    using container_type        = _Container;

protected:

    _Container c;

public:

    queue() :
        queue(_Container())
    {
    }

    explicit queue(const _Container& __c) :
        c(__c)
    {
    }

    explicit queue(_Container&& __c) :
        c(std::move(__c))
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    explicit queue(const _Alloc& __a) :
        c(__a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    queue(const _Container& __c, const _Alloc& __a) :
        c(__c, __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    queue(_Container&& __c, const _Alloc& __a) :
        c(std::move(__c), __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    queue(const queue& __q, const _Alloc& __a) :
        c(__q.c, __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    queue(queue&& __q, const _Alloc& __a) :
        c(std::move(__q.c), __a)
    {
    }

    [[nodiscard]] bool empty() const
    {
        return c.empty();
    }

    size_type size() const
    {
        return c.size();
    }

    reference front()
    {
        return c.front();
    }

    const_reference front() const
    {
        return c.front();
    }

    reference back()
    {
        return c.back();
    }

    const_reference back() const
    {
        return c.back();
    }

    void push(const value_type& __x)
    {
        c.push_back(__x);
    }

    void push(value_type&& __x)
    {
        c.push_back(std::move(__x));
    }

    template <class... _Args>
    decltype(auto) emplace(_Args&&... __args)
    {
        return c.emplace_back(std::forward<_Args>(__args)...);
    }

    void pop()
    {
        c.pop_front();
    }

    void swap(queue& __q)
        noexcept(is_nothrow_swappable_v<_Container>)
    {
        using std::swap;
        swap(c, __q.c);
    }
};

template <class _Container>
queue(_Container)
    -> queue<typename _Container::value_type, _Container>;

template <class _Container, class _Allocator>
queue(_Container, _Allocator)
    -> queue<typename _Container::value_type, _Container>;


template <class _T, class _Container>
bool operator==(const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c == __y.c;
}

template <class _T, class _Container>
bool operator!=(const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c != __y.c;
}

template <class _T, class _Container>
bool operator< (const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c < __y.c;
}

template <class _T, class _Container>
bool operator> (const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c > __y.c;
}

template <class _T, class _Container>
bool operator<=(const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c <= __y.c;
}

template <class _T, class _Container>
bool operator>=(const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c >= __y.c;
}

template <class _T, class _Container>
compare_three_way_result_t<_Container> operator<=>(const queue<_T, _Container>& __x, const queue<_T, _Container>& __y)
{
    return __x.c <=> __y.c;
}


template <class _T, class _Container>
    requires is_swappable_v<_Container>
void swap(queue<_T, _Container>& __x, queue<_T, _Container>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Container, class _Allocator>
struct uses_allocator<queue<_T, _Container>, _Allocator> :
    uses_allocator<_Container, _Allocator>::type {};


template <class _T, class _Container = vector<_T>, class _Compare = less<typename _Container::value_type>>
class priority_queue
{
public:

    using value_type            = typename _Container::value_type;
    using reference             = typename _Container::reference;
    using const_reference       = typename _Container::const_reference;
    using size_type             = typename _Container::size_type;
    using container_type        = _Container;
    using value_compare         = _Compare;

protected:

    _Container      c;
    [[no_unique_address]] _Compare comp;

public:

    priority_queue() :
        priority_queue(_Compare())
    {
    }

    explicit priority_queue(const _Compare& __x) :
        priority_queue(__x, _Container())
    {
    }

    priority_queue(const _Compare& __x, const _Container& __c) :
        c(__c), comp(__x)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    priority_queue(const _Compare& __x, _Container&& __c) :
        c(std::move(__c)), comp(__x)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    priority_queue(_InputIterator __first, _InputIterator __last, const _Compare& __x, const _Container& __c) :
        c(__c), comp(__x)
    {
        c.insert(c.end(), __first, __last);
        make_heap(c.begin(), c.end(), comp);
    }

    template <class _InputIterator>
        requires __detail::__cpp17_input_iterator<_InputIterator>
    priority_queue(_InputIterator __first, _InputIterator __last, const _Compare& __x, _Container&& __c = _Container()) :
        c(std::move(__c)), comp(__x)
    {
        c.insert(c.end(), __first, __last);
        make_heap(c.begin(), c.end(), comp);
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    explicit priority_queue(const _Alloc& __a) :
        c(__a), comp()
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    priority_queue(const _Compare& __x, const _Alloc& __a) :
        c(__a), comp(__x)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    priority_queue(const _Compare& __x, const _Container& __c, const _Alloc& __a) :
        c(__c, __a), comp(__x)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    priority_queue(const _Compare& __x, _Container&& __c, const _Alloc& __a) :
        c(std::move(__c), __a), comp(__x)
    {
        make_heap(c.begin(), c.end(), comp);
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    priority_queue(const priority_queue& __q, const _Alloc& __a) :
        c(__q.c, __a), comp(__q.comp)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    priority_queue(priority_queue&& __q, const _Alloc& __a) :
        c(std::move(__q.c), __a), comp(std::move(__q.comp))
    {
    }

    [[nodiscard]] bool empty() const
    {
        return c.empty();
    }

    size_type size() const
    {
        return c.size();
    }

    const_reference top() const
    {
        return c.front();
    }

    void push(const value_type& __x)
    {
        c.push_back(__x);
        push_heap(c.begin(), c.end(), comp);
    }

    void push(value_type&& __x)
    {
        c.push_back(std::move(__x));
        push_heap(c.begin(), c.end(), comp);
    }

    template <class... _Args>
    void emplace(_Args&&... __args)
    {
        c.emplace_back(std::forward<_Args>(__args)...);
        push_heap(c.begin(), c.end(), comp);
    }

    void pop()
    {
        pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }

    void swap(priority_queue& __q)
        noexcept(is_nothrow_swappable_v<_Container> && is_nothrow_swappable_v<_Compare>)
    {
        using std::swap;
        swap(c, __q.c);
        swap(comp, __q.comp);
    }
};

template <class _Compare, class _Container>
priority_queue(_Compare, _Container)
    -> priority_queue<typename _Container::value_type, _Container, _Compare>;

template <class _InputIterator, class _Compare = less<typename iterator_traits<_InputIterator>::value_type>, class _Container = vector<typename iterator_traits<_InputIterator>::value_type>>
    requires __detail::__cpp17_input_iterator<_InputIterator>
priority_queue(_InputIterator, _InputIterator, _Compare = _Compare(), _Container = _Container())
    -> priority_queue<typename iterator_traits<_InputIterator>::value_type, _Container, _Compare>;

template <class _Compare, class _Container, class _Allocator>
priority_queue(_Compare, _Container, _Allocator)
    -> priority_queue<typename _Container::value_type, _Container, _Compare>;


template <class _T, class _Container, class _Compare>
    requires is_swappable_v<_Container> && is_swappable_v<_Compare>
void swap(priority_queue<_T, _Container, _Compare>& __x, priority_queue<_T, _Container, _Compare>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Container, class _Compare, class _Allocator>
struct uses_allocator<priority_queue<_T, _Container, _Compare>, _Allocator> :
    uses_allocator<_Container, _Allocator>::type {};


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_QUEUE_H */

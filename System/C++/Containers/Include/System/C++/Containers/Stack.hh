#ifndef __SYSTEM_CXX_CONTAINERS_STACK_H
#define __SYSTEM_CXX_CONTAINERS_STACK_H


#include <System/C++/Containers/Private/Config.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/Utility/Memory.hh>

#include <System/C++/Containers/Deque.hh>


namespace __XVI_STD_CONTAINERS_NS
{


template <class _T, class _Container = deque<_T>>
class stack
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

    stack() :
        stack(_Container())
    {
    }

    stack(const _Container& __c) :
        c(__c)
    {
    }

    stack(_Container&& __c) :
        c(std::move(__c))
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    explicit stack(const _Alloc& __a) :
        c(__a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    stack(const _Container& __c, const _Alloc& __a) :
        c(__c, __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    stack(_Container&& __c, const _Alloc& __a) :
        c(std::move(__c), __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    stack(const stack& __s, const _Alloc& __a) :
        c(__s.c, __a)
    {
    }

    template <class _Alloc>
        requires uses_allocator_v<container_type, _Alloc>
    stack(stack&& __s, const _Alloc& __a) :
        c(std::move(__s.c), __a)
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

    reference top()
    {
        return c.back();
    }

    const_reference top() const
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
        c.pop_back();
    }

    void swap(stack& __s)
        noexcept(is_nothrow_swappable_v<_Container>)
    {
        using std::swap;
        swap(c, __s.c);
    }
};

template <class _Container>
stack(_Container)
    -> stack<typename _Container::value_type, _Container>;

template <class _Container, class _Allocator>
    requires __detail::__is_allocator<_Allocator>
stack(_Container, _Allocator)
    -> stack<typename _Container::value_type, _Container>;

template <class _T, class _Container, class _Alloc>
struct uses_allocator<stack<_T, _Container>, _Alloc> :
    uses_allocator<_Container, _Alloc>::type {};


template <class _T, class _Container>
bool operator==(const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c == __y.c;
}

template <class _T, class _Container>
bool operator!=(const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c != __y.c;
}

template <class _T, class _Container>
bool operator< (const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c < __y.c;
}

template <class _T, class _Container>
bool operator> (const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c > __y.c;
}

template <class _T, class _Container>
bool operator<=(const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c <= __y.c;
}

template <class _T, class _Container>
bool operator>=(const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c >= __y.c;
}

template <class _T, class _Container>
compare_three_way_result_t<_Container> operator<=>(const stack<_T, _Container>& __x, const stack<_T, _Container>& __y)
{
    return __x.c <=> __y.c;
}


template <class _T, class _Container>
    requires is_swappable_v<_Container>
void swap(stack<_T, _Container>& __x, stack<_T, _Container>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T, class _Container, class _Allocator>
struct uses_allocator<stack<_T, _Container>, _Allocator>;


} // namespace __XVI_STD_CONTAINERS_NS


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_STACK_H */

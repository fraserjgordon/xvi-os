#ifndef __SYSTEM_CXX_CONTAINERS_PRIVATE_ALLOCATOR_H
#define __SYSTEM_CXX_CONTAINERS_PRIVATE_ALLOCATOR_H


#include <System/C++/Containers/Private/Config.hh>

#include <System/C++/TypeTraits/TypeTraits.hh>


namespace __XVI_STD_CONTAINERS_NS::__detail
{


template <class _A>
concept __is_allocator = requires(_A& __a)
{
    typename _A::value_type;
    { __a.allocate(std::size_t{}) };
};


} // namespace __XVI_STD_CONTAINERS_NS::__detail


#endif /* ifndef __SYSTEM_CXX_CONTAINERS_PRIVATE_ALLOCATOR_H */

#ifndef __SYSTEM_CXX_CORE_ITERATORTRAITS_H
#define __SYSTEM_CXX_CORE_ITERATORTRAITS_H


#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class _T> struct iterator_traits;


struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};



} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_ITERATORTRAITS_H */
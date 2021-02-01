#pragma once
#ifndef __SYSTEM_CXX_UTILITY_STDLIB_VOIDPTR_ITER_H
#define __SYSTEM_CXX_UTILITY_STDLIB_VOIDPTR_ITER_H


#include <System/C++/Utility/Private/Config.hh>

#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Iterator.hh>


namespace __XVI_STD_UTILITY_NS
{


struct voidptr_iter
{
    using value_type        = byte;
    using difference_type   = make_signed_t<size_t>;

    value_type& operator*() const
    {
        return const_cast<value_type&>(*reinterpret_cast<const value_type*>(m_ptr));
    }

    voidptr_iter& operator++()
    {
        adjust(1);
        return *this;
    }

    voidptr_iter operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    voidptr_iter& operator--()
    {
        adjust(-1);
        return *this;
    }

    voidptr_iter operator--(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    voidptr_iter operator+(difference_type n) const
    {
        auto tmp = *this;
        tmp.adjust(n);
        return tmp;
    }

    voidptr_iter operator-(difference_type n) const
    {
        return operator+(-n);
    }

    difference_type operator-(voidptr_iter i) const
    {
        return (reinterpret_cast<intptr_t>(m_ptr) - reinterpret_cast<intptr_t>(i.m_ptr)) / m_size;
    }

    voidptr_iter& operator+=(difference_type n)
    {
        *this = operator+(n);
        return *this;
    }

    voidptr_iter& operator-=(difference_type n)
    {
        return operator+=(-n);
    }

    value_type& operator[](difference_type n) const
    {
        return *(*this + n);
    }

    friend voidptr_iter operator+(difference_type n, voidptr_iter i)
    {
        return i + n;
    }

    void adjust(difference_type n)
    {
        m_ptr = reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(m_ptr) + n * m_size);
    }

    friend strong_ordering operator<=>(const voidptr_iter& a, const voidptr_iter& b)
    {
        return a.m_ptr <=> b.m_ptr;
    }

    friend bool operator==(const voidptr_iter&, const voidptr_iter&) = default;

    const void*     m_ptr = nullptr;
    difference_type m_size = 0;
};

#if __cpp_concepts
static_assert(bidirectional_iterator<voidptr_iter>);
#endif


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_STDLIB_VOIDPTR_ITER_H */

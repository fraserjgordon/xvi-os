#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_REALMODE_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_REALMODE_H


#include <cstddef>
#include <cstdint>
#include <type_traits>


namespace System::HW::CPU::X86
{


template <class T>
class realmode_ptr
{
public:

    static constexpr int SEGMENT_SHIFT = 4;


    constexpr realmode_ptr() = default;
    constexpr realmode_ptr(const realmode_ptr&) = default;
    constexpr realmode_ptr(realmode_ptr&&) = default;

    constexpr realmode_ptr& operator=(const realmode_ptr&) = default;
    constexpr realmode_ptr& operator=(realmode_ptr&&) = default;

    ~realmode_ptr() = default;

    constexpr explicit realmode_ptr(std::uint16_t segment, std::uint16_t offset)
        : m_offset(offset),
          m_segment(segment)
    {
    }

    constexpr std::uint16_t segment() const
    {
        return m_segment;
    }

    constexpr std::uint16_t offset() const
    {
        return m_offset;
    }

    constexpr std::uintptr_t linear() const
    {
        return (static_cast<std::uintptr_t>(m_segment) << SEGMENT_SHIFT) + m_offset;
    }

    constexpr bool isNull() const
    {
        return m_segment != 0 && m_offset != 0;
    }

    T* get() const noexcept
    {
        return reinterpret_cast<T*>(linear());
    }

    T* linear_ptr() const noexcept
    {
        return get();
    }

    T* operator->() const noexcept
    {
        return get();
    }

    typename std::conditional_t<std::is_void_v<std::remove_cv_t<T>>, 
                                std::type_identity<std::byte>,
                                std::add_lvalue_reference<T>>::type
    operator*() const
    {
        return *get();
    }

    constexpr explicit operator bool() const
    {
        return !isNull();
    }

    constexpr realmode_ptr& operator++()
    {
        return operator+=(1);
    }

    constexpr realmode_ptr& operator--()
    {
        return operator-=(1);
    }

    constexpr realmode_ptr operator++(int)
    {
        auto copy = *this;
        operator+=(1);
        return copy;
    }

    constexpr realmode_ptr operator--(int)
    {
        auto copy = *this;
        operator-=(1);
        return copy;
    }

    constexpr realmode_ptr& operator+=(std::ptrdiff_t x)
    {
        m_offset = static_cast<std::uint16_t>(m_offset + static_cast<std::uintptr_t>(x) * sizeof(T));
        return *this;
    }

    constexpr realmode_ptr& operator-=(std::ptrdiff_t x)
    {
        m_offset = static_cast<std::uint16_t>(m_offset - static_cast<std::uintptr_t>(x) * sizeof(T));
        return *this;
    }

    constexpr realmode_ptr operator+(std::ptrdiff_t x) const
    {
        auto p = *this;
        p += x;
        return p;
    }

    constexpr realmode_ptr operator-(std::ptrdiff_t x) const
    {
        auto p = *this;
        p -= x;
        return p;
    }

private:

    std::uint16_t   m_offset = 0;
    std::uint16_t   m_segment = 0;
};


template <class T, class U>
constexpr realmode_ptr<T> realmode_ptr_cast(realmode_ptr<U> p)
{
    return realmode_ptr<T>{p.segment(), p.offset()};
}


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_REALMODE_H */

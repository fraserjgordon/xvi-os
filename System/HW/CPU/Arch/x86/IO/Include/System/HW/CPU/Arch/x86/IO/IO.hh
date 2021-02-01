#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_IO_IO_H
#define __SYSTEM_HW_CPU_ARCH_X86_IO_IO_H


#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Span.hh>


namespace System::HW::CPU::X86
{


inline std::uint8_t inb(std::uint16_t port)
{
    std::uint8_t value;
    asm volatile
    (
        "inb    %1, %b0"
        : "=a" (value)
        : "Nd" (port)
        : "memory"
    );

    return value;
}

inline std::uint16_t inw(std::uint16_t port)
{
    std::uint16_t value;
    asm volatile
    (
        "inw    %1, %w0"
        : "=a" (value)
        : "Nd" (port)
        : "memory"
    );

    return value;
}

inline std::uint32_t inl(std::uint16_t port)
{
    std::uint32_t value;
    asm volatile
    (
        "inl    %1, %k0"
        : "=a" (value)
        : "Nd" (port)
        : "memory"
    );

    return value;
}

inline void outb(std::uint16_t port, std::uint8_t value)
{
    asm volatile
    (
        "outb   %b0, %1"
        :
        : "a" (value), "Nd" (port)
        : "memory"
    );
}

inline void outw(std::uint16_t port, std::uint16_t value)
{
    asm volatile
    (
        "outw   %w0, %1"
        :
        : "a" (value), "Nd" (port)
        : "memory"
    );
}

inline void outl(std::uint16_t port, std::uint32_t value)
{
    asm volatile
    (
        "outl   %k0, %1"
        :
        : "a" (value), "Nd" (port)
        : "memory"
    );
}


inline void insb(std::uint16_t port, std::span<std::uint8_t> dest)
{
    asm volatile
    (
        "rep insb"
        :
        : "d" (port), "D" (dest.data()), "c" (dest.size())
        : "memory"
    );
}

inline void insw(std::uint16_t port, std::span<std::uint16_t> dest)
{
    asm volatile
    (
        "rep insw"
        :
        : "d" (port), "D" (dest.data()), "c" (dest.size())
        : "memory"
    );
}

inline void insl(std::uint16_t port, std::span<std::uint32_t> dest)
{
    asm volatile
    (
        "rep insl"
        :
        : "d" (port), "D" (dest.data()), "c" (dest.size())
        : "memory"
    );
}

inline void outsb(std::uint16_t port, std::span<const std::uint8_t> src)
{
    asm volatile
    (
        "rep outsb"
        :
        : "d" (port), "S" (src.data()), "c" (src.size())
        : "memory"
    );
}

inline void outsw(std::uint16_t port, std::span<const std::uint16_t> src)
{
    asm volatile
    (
        "rep outsw"
        :
        : "d" (port), "S" (src.data()), "c" (src.size())
        : "memory"
    );
}

inline void outsl(std::uint16_t port, std::span<const std::uint32_t> src)
{
    asm volatile
    (
        "rep outsl"
        :
        : "d" (port), "S" (src.data()), "c" (src.size())
        : "memory"
    );
}


template <class ObjectType, class AccessType = ObjectType>
class IOPort
{
private:

    class IOPortRef;

public:

    static_assert(sizeof(ObjectType) % sizeof(AccessType) == 0);
    static_assert(sizeof(AccessType) == 1 || sizeof(AccessType) == 2 || sizeof(AccessType) == 4);


    constexpr IOPort() = default;
    constexpr IOPort(const IOPort&) = default;
    constexpr IOPort(IOPort&&) = default;

    constexpr IOPort& operator=(const IOPort&) = default;
    constexpr IOPort& operator=(IOPort&&) = default;

    ~IOPort() = default;


    IOPortRef get() const
    {
        return IOPortRef{*this};
    }

    IOPortRef operator*() const
    {
        return get();
    }

    ObjectType read() const
    {
        ObjectType value;
        
        constexpr auto count = sizeof(ObjectType) / sizeof(AccessType);
        std::span<AccessType> span{reinterpret_cast<AccessType*>(std::addressof(value)), count};

        if constexpr (count == 1)
        {
            if constexpr (sizeof(AccessType) == 1)
                span.front() = inb(m_port);
            else if constexpr (sizeof(AccessType) == 2)
                span.front() = inw(m_port);
            else
                span.front() = inl(m_port);
        }
        else
        {
            if constexpr (sizeof(AccessType) == 1)
                insb(m_port, span);
            else if constexpr (sizeof(AccessType) == 2)
                insw(m_port, span);
            else
                insl(m_port, span);
        }
    }

    void write(const ObjectType& obj) const
    {
        constexpr auto count = sizeof(ObjectType) / sizeof(AccessType);
        std::span<AccessType> span{reinterpret_cast<const AccessType*>(std::addressof(obj)), count};

        if constexpr (count == 1)
        {
            if constexpr (sizeof(AccessType) == 1)
                outb(m_port, span.front());
            else if constexpr (sizeof(AccessType) == 2)
                outw(m_port, span.front());
            else
                outl(m_port, span.front());
        }
        else
        {
            if constexpr (sizeof(AccessType) == 1)
                outsb(m_port, span);
            else if constexpr (sizeof(AccessType) == 2)
                outsw(m_port, span);
            else
                outsl(m_port, span);
        }
    }

private:

    class IOPortRef
    {
    public:

        IOPortRef(const IOPortRef&) = delete;
        IOPortRef(IOPortRef&&) = delete;
        IOPortRef& operator=(const IOPortRef&) = delete;
        IOPortRef& operator=(IOPortRef&) = delete;

        ~IOPortRef() = default;

        constexpr explicit IOPortRef(const IOPort& port)
            : m_port(port)
        {
        }

        IOPortRef& operator=(const ObjectType& obj)
        {
            m_port.write(obj);
        }

        operator ObjectType() const
        {
            return m_port.read();
        }

    private:

        const IOPort& m_port;
    };


    std::uint16_t   m_port = 0;
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_IO_IO_H */

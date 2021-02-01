#ifndef __SYSTEM_BOOT_IGNITER_MEMORY_BITMAP_H
#define __SYSTEM_BOOT_IGNITER_MEMORY_BITMAP_H


#include <bitset>
#include <cstddef>
#include <cstdint>
#include <limits>


namespace System::Boot::Igniter
{


class PageBitmap
{
public:

    static constexpr std::size_t PageSize   = 4096;

    // Constructs a page bitmap using a block of statically-allocated memory.
    template <std::size_t N>
    consteval PageBitmap(std::uintptr_t base, std::bitset<N>& bits)
    requires (N >= 32) :
        m_baseAddress(base),
        m_length(N * PageSize),
        m_bitmap(&bits)
    {
    }

    void markAvailable(std::uintptr_t base, std::size_t size);
    void markUnavailable(std::uintptr_t base, std::size_t size);

    void markAllAvailable()
    {
        markAvailable(m_baseAddress, m_length);
    }

    void markAllUnavailable()
    {
        markUnavailable(m_baseAddress, m_length);
    }

    std::uintptr_t allocatePage();
    void freePage(std::uintptr_t address);

    std::size_t getAvailableSize(std::uintptr_t address_limit = ~std::uintptr_t(0)) const;

private:

    using element_t = std::bitset<std::numeric_limits<std::uintptr_t>::digits>;
    static_assert(sizeof(element_t) == sizeof(std::uintptr_t));

    std::uintptr_t      m_baseAddress   = 0;
    std::size_t         m_length        = 0;
    void*               m_bitmap        = nullptr;

    void markRange(std::uintptr_t base, std::size_t size, bool available);
    void adjustBaseAndSize(std::uintptr_t& base, std::size_t& size);
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_MEMORY_BITMAP_H */

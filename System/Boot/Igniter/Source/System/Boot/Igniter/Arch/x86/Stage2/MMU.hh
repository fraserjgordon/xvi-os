#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H


#include <cstdint>
#include <memory>
#include <type_traits>

#include <System/Kernel/Arch/x86/MMU/Types.hh>
#include <System/Kernel/Arch/x86/MMU/UserCopy.hh>


namespace System::Boot::Igniter
{


using Kernel::X86::MMU::paddr_t;
using Kernel::X86::MMU::psize_t;
using Kernel::X86::MMU::vaddr_t;
using Kernel::X86::MMU::vsize_t;

using Kernel::X86::MMU::page_flags;
using Kernel::X86::MMU::cache_type;
using Kernel::X86::MMU::CacheType;

namespace PageFlag = Kernel::X86::MMU::PageFlag;


using Kernel::X86::MMU::copyFromUserUnchecked;
using Kernel::X86::MMU::copyToUserUnchecked;


// Flags for the addEarlyMap method.
namespace EarlyMapFlag
{
    constexpr std::uint32_t     R   = 0;
    constexpr std::uint32_t     W   = 0x01;
    constexpr std::uint32_t     X   = 0x02;
    constexpr std::uint32_t     U   = 0x04;
    constexpr std::uint32_t     C   = 0x08;

    constexpr std::uint32_t     RWXC    = R|W|X|C;
    constexpr std::uint32_t     RC      = R|C;
    constexpr std::uint32_t     RW      = R|W;
    constexpr std::uint32_t     RXC     = R|X|C;
    constexpr std::uint32_t     RWXUC   = R|W|X|U|C;
    constexpr std::uint32_t     RXUC    = R|X|U|C;
}

using early_map_flag_t = std::uint32_t;


class AutoMap
{
public:

    constexpr AutoMap() = default;
    AutoMap(const AutoMap&) = delete;
    AutoMap(AutoMap&&);

    AutoMap(paddr_t physical, psize_t size, page_flags = 0, cache_type cache = CacheType::WriteBack);
    AutoMap(paddr_t physical, vsize_t size, page_flags flags, cache_type cache, vaddr_t where);

    ~AutoMap()
    {
        reset();
    }

    AutoMap& operator=(const AutoMap&) = delete;
    AutoMap& operator=(AutoMap&&);


    void reset();

    paddr_t physical() const noexcept
    {
        return m_physical;
    }

    vaddr_t base() const noexcept
    {
        return m_virtual;
    }

    vsize_t size() const noexcept
    {
        return m_size;
    }

    vsize_t offset() const noexcept
    {
        return m_offset;
    }

    vaddr_t target() const noexcept
    {
        return m_virtual + m_offset;
    }


private:

    paddr_t                 m_physical = 0;
    vaddr_t                 m_virtual = 0;
    vsize_t                 m_size = 0;
    vsize_t                 m_offset = 0;
};


template <class T>
class AutoMapped :
    public AutoMap
{
public:

    using AutoMap::AutoMap;


    T* get() const noexcept
    {
        return reinterpret_cast<T*>(target());
    }

    T& operator*() const
        requires (!std::is_void_v<T>)
    {
        return *get();
    }

    T* operator->() const
        requires (!std::is_array_v<T>)
    {
        return get();
    }

    auto& operator[](std::size_t offset) const
        requires (std::is_array_v<T>)
    {
        return get()[offset];
    }
};


void enablePaging();

// Called by the probe code to set up the initial set of (identity-mapped) page tables.
void addEarlyMap(std::uint32_t address, std::uint32_t size, early_map_flag_t flags);

// Once paging has been enabled, this method can be used to add new mappings to the page tables.
void addMap(vaddr_t address, vsize_t size, paddr_t physical, page_flags flags, cache_type caching);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H */

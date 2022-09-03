#ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_INITPAGETABLE_H
#define __SYSTEM_HW_CPU_ARCH_X86_MMU_INITPAGETABLE_H


#include <cstdint>

#include <System/HW/CPU/Arch/x86/MMU/Types.hh>


namespace System::HW::CPU::X86::MMU
{


class MMU;


// Class representing an initial page table (i.e the page table created before paging is activated). This page table
// requires special handling as the table is accessed via direct physical addresses rather than the normal recursive-
// mapping.
//
// Most of the features of a normal page table are not supported.
class InitPageTable
{
public:

    constexpr InitPageTable() = default;
    constexpr InitPageTable(const InitPageTable&) = default;

    constexpr InitPageTable(MMU& mmu, std::uint64_t root) :
        m_root{root},
        m_mmu{&mmu}
    {
    }

    constexpr ~InitPageTable() = default;

    constexpr InitPageTable& operator=(const InitPageTable&) = default;


    bool addMapping(std::uint64_t vaddr, std::uint64_t size, std::uint64_t paddr, page_flags, cache_type);

    std::uint64_t root() const noexcept
    {
        return m_root;
    }

    MMU* mmu() const noexcept
    {
        return m_mmu;
    }

private:

    std::uint64_t       m_root = 0;
    MMU*                m_mmu = nullptr;
};


} // namespace System::HW::CPU::X86::MMU


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_INITPAGETABLE_H */

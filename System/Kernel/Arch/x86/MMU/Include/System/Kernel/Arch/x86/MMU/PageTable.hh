#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H


#include <cstdint>
#include <utility>

#include <System/Kernel/Arch/x86/MMU/Types.hh>


namespace System::Kernel::X86::MMU
{


class MappedPageTable;
class MMU;


class PageTable
{
    friend class MMU;

public:

    static constexpr vsize_t PageSize = 4096;


    constexpr PageTable() = default;
    PageTable(const PageTable&) = delete;
    
    constexpr PageTable(PageTable&&) = default;

    ~PageTable();

    PageTable& operator=(const PageTable&) = delete;

    PageTable& operator=(PageTable&&) = default;


    // Maps this page table into the current page table at the given virtual address, thereby making it accessible for
    // examination and modification.
    //
    // This operation will fail if the given address is already mapped.
    MappedPageTable map(vaddr_t address) &&;

protected:

    MMU*        m_mmu = nullptr;
    paddr_t     m_root = 0;
    std::size_t m_recurseIndex = 0;     // Which entry in this table recursively maps itself.


    PageTable(MMU& mmu, paddr_t root, std::size_t recurse_index) :
        m_mmu{&mmu},
        m_root{root},
        m_recurseIndex(recurse_index)
    {
    }
};


class MappedPageTable :
    public PageTable
{
    friend class MMU;
    friend class PageTable;

public:

    constexpr MappedPageTable() = default;
    MappedPageTable(const MappedPageTable&) = delete;

    constexpr MappedPageTable(MappedPageTable&& other) :
        PageTable(std::move(static_cast<PageTable&&>(other))),
        m_address(std::exchange(other.m_address, 0))
    {
    }

    ~MappedPageTable();

    MappedPageTable& operator=(const MappedPageTable&) = delete;

    MappedPageTable& operator=(MappedPageTable&& other)
    {
        PageTable::operator=(static_cast<PageTable&&>(other));
        m_address = std::exchange(other.m_address, 0);

        return *this;
    }

    constexpr bool operator==(const MappedPageTable&) const noexcept = default;


    // Returns the virtual address at which this page table is mapped.
    std::uintptr_t address() const noexcept;

    // Unmaps the page table and reinitialises this object to the default state.
    void reset() noexcept;

    // Looks up the mapping for the given virtual address.
    mapping_info lookup(vaddr_t address) const noexcept;

    // Adds a new mapping to this page table.
    bool add(const mapping_info&) noexcept;

    // Removes a mapping from this page table.
    //
    // Note: if removing the mapping requires a large page to be split, this method may require memory allocation!
    bool remove(vaddr_t address, vsize_t size = PageSize) noexcept;

    // Changes the flags or permissions on a mapping.
    bool changeFlags(page_flags, vaddr_t address, vsize_t size = PageSize) noexcept;


private:

    vaddr_t     m_address;      // Address at which this page table is mapped.


    MappedPageTable(PageTable&& table, vaddr_t address) :
        PageTable{std::move(table)},
        m_address{address}
    {
    }
};


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H */

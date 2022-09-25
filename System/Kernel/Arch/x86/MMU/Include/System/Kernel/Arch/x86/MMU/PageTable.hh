#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H


#include <cstdint>

#include <System/Kernel/Arch/x86/MMU/Types.hh>


namespace System::Kernel::X86::MMU
{


class MappedPageTable;


class PageTable
{
public:

    static constexpr vsize_t PageSize = 4096;


    // Maps this page table into the current page table at the given virtual address, thereby making it accessible for
    // examination and modification.
    //
    // This operation will fail if the given address is already mapped.
    MappedPageTable map(vaddr_t address);

private:
};


class MappedPageTable :
    public PageTable
{
public:

    MappedPageTable() = default;
    MappedPageTable(const MappedPageTable&) = delete;
    MappedPageTable(MappedPageTable&&) = default;

    ~MappedPageTable();

    MappedPageTable& operator=(const MappedPageTable&) = delete;
    MappedPageTable& operator=(MappedPageTable&&) = default;

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
    bool remove(vaddr_t address, vsize_t size = PageSize, bool remove_empty_tables = true) noexcept;

    // Changes the flags or permissions on a mapping.
    bool changeFlags(page_flags, vaddr_t address, vsize_t size = PageSize) noexcept;


private:

    vaddr_t     m_address;      // Address at which this page table is mapped.
};


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLE_H */

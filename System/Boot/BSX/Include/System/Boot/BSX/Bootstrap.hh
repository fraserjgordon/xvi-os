#ifndef __SYSTEM_BOOT_BSX_BOOTSTRAP_H
#define __SYSTEM_BOOT_BSX_BOOTSTRAP_H


namespace System::Boot::BSX
{


// Record describing one or more pages of memory available to use while bootstrapping the kernel.
struct free_memory_range
{
    std::uint64_t       base = 0;       // Base address of range.
    std::uint64_t       length = 0;     // Size of the free range.
};


struct 


} // namespace System::Boot::BSX


#endif /* ifndef __SYSTEM_BOOT_BSX_BOOTSTRAP_H */

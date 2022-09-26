#include <System/Kernel/Runpatch/Arch/x86/Runpatch.hh>

#include <cstring>
#include <span>


namespace System::Kernel::Runpatch::X86
{


// Symbols defined in the link script that demarcate the runpatch control table.
extern const control _RUNPATCH_CONTROL_START[] asm("_RUNPATCH_CONTROL_START");
extern const control _RUNPATCH_CONTROL_END[] asm("_RUNPATCH_CONTROL_END");


// Utility function to fill space with NOP instructions.
static void nopFill(std::uintptr_t where, std::size_t length)
{
    //! @todo use multibyte NOPs where appropriate.
    std::memset(reinterpret_cast<void*>(where), 0x90, length);
}


void applyRunpatch(std::string_view key, unsigned int alternative)
{
    // How many entries are in the control table?
    auto count = _RUNPATCH_CONTROL_END - _RUNPATCH_CONTROL_START;
    std::span table {_RUNPATCH_CONTROL_START, count};

    // Scan the table for entries matching the key.
    for (const auto& entry : table)
    {
        if (entry.key != key)
            continue;

        // Which alternative are we applying?
        const alternative_info* info;
        if (alternative == 0)
            info = &entry.original;
        else
            info = &entry.alternatives[alternative - 1];

        // Copy the code for the alternative into place.
        std::memcpy(reinterpret_cast<void*>(entry.ip), reinterpret_cast<const void*>(info->location), info->size);

        // Fill any unused space with NOPs.
        auto space = entry.length - info->size;
        nopFill(entry.ip + info->size, space);
    }
}


} // namespace System::Kernel::Runpatch::X86

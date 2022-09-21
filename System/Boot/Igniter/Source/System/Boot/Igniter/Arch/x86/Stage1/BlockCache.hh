#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BLOCKCACHE_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BLOCKCACHE_H


#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/SharedPtr.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/MemoryLayout.hh>


namespace System::Boot::Igniter
{


class BlockCache
{
public:

    static constexpr std::size_t BlockSize = 512;

    using block_ptr = std::shared_ptr<std::byte[BlockSize]>;


    std::pair<bool, block_ptr> findOrAllocate(std::uint64_t lba);

    static BlockCache& instance();

private:

    static constexpr auto CacheSize = BLOCK_CACHE_SIZE / BlockSize;

    struct block_descriptor
    {
        std::uint64_t               lba = 0;
        block_ptr                   ptr = {};
        std::uint32_t               lru = 0;
    };

    struct block_deleter
    {
        void operator()(block_descriptor*);
    };


    std::array<block_descriptor, CacheSize>     m_descriptors;
    std::uint32_t                               m_lruCounter = 0;


    static BlockCache s_instance;
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BLOCKCACHE_H */

#include <System/Boot/Igniter/Arch/x86/Stage1/BlockCache.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>


namespace System::Boot::Igniter
{


BlockCache BlockCache::s_instance = {};


std::pair<bool, BlockCache::block_ptr> BlockCache::findOrAllocate(std::uint64_t lba)
{
    block_descriptor* empty = nullptr;
    for (std::size_t i = 0; i < CacheSize; ++i)
    {
        // Is this the entry we're looking for?
        if (m_descriptors[i].lba == lba && m_descriptors[i].ptr)
        {
            m_descriptors[i].lru = ++m_lruCounter;
            return {true, m_descriptors[i].ptr};
        }

        // Is this a candidate for allocation?
        if (m_descriptors[i].ptr == nullptr)
        {
            // Use this if we don't have a candidate or the candidate is less good.
            if (empty == nullptr || empty->ptr != nullptr)
                empty = &m_descriptors[i];
        }
        else if (m_descriptors[i].ptr.use_count() <= 1)
        {
            // This slot is in use but could be viable as nobody holds a reference. We'll only use it if it is less
            // recently used than our current selected victim (or if we don't have a victim yet).
            if (empty == nullptr || empty->lru > m_descriptors[i].lru)
                empty = &m_descriptors[i];
        }
    }

    // The block isn't in the cache yet.
    //! @todo what if we couldn't find an empty slot?

    auto index = empty - &m_descriptors[0];

    // We manage the lifetime of the block in the cache by creating a shared_ptr with the following characteristics:
    //  - originally points to the descriptor
    //  - uses a deleter that does nothing
    //  - copied but with the copy pointing to the space for the block, not the descriptor
    auto block = reinterpret_cast<std::byte*>(BLOCK_CACHE_START + index * BlockSize);
    std::shared_ptr<block_descriptor> original_ptr(empty, block_deleter{});
    std::shared_ptr<std::byte[BlockSize]> returned_ptr(original_ptr, block);
    empty->lba = lba;
    empty->ptr = returned_ptr;
    empty->lru = ++m_lruCounter;

    return {false, returned_ptr};
}


BlockCache& BlockCache::instance()
{
    return s_instance;
}


void BlockCache::block_deleter::operator()(block_descriptor*)
{
    // Does nothing.
}


}

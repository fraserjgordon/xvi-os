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
            //log(priority::trace, "BlockCache: cache hit for LBA {} in slot {}", lba, i);
            return {true, m_descriptors[i].ptr};
        }

        // Is this a candidate for allocation?
        if (m_descriptors[i].ptr == nullptr)
        {
            // Use this if we don't have a candidate or the candidate is less good.
            if (empty == nullptr || empty->ptr != nullptr)
                empty = &m_descriptors[i];
        }
        else if (m_descriptors[i].ptr.use_count() <= 1 && empty == nullptr)
        {
            // This slot is in use but could be viable as nobody holds a reference.
            empty = &m_descriptors[i];
        }
    }

    // The block isn't in the cache yet.
    //! @todo what if we couldn't find an empty slot?

    auto index = empty - &m_descriptors[0];
    if (empty->ptr)
    {
        //log(priority::trace, "BlockCache: evicting LBA {} from slot {} in favour of {}", empty->lba, index, lba);
    }
    else
    {
        //log(priority::trace, "BlockCache: inserting LBA {} into unused slot {}", lba, index);
    }

    // We manage the lifetime of the block in the cache by creating a shared_ptr with the following characteristics:
    //  - originally points to the descriptor
    //  - uses a deleter that does nothing
    //  - copied but with the copy pointing to the space for the block, not the descriptor
    auto block = reinterpret_cast<std::byte*>(BLOCK_CACHE_START + index * BlockSize);
    std::shared_ptr<block_descriptor> original_ptr(empty, block_deleter{});
    std::shared_ptr<std::byte[BlockSize]> returned_ptr(original_ptr, block);
    empty->lba = lba;
    empty->ptr = returned_ptr;

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

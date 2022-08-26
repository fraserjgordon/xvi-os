#include <System/Boot/Igniter/Memory/Bitmap.hh>


namespace System::Boot::Igniter
{


void PageBitmap::markAvailable(std::uintptr_t base, std::size_t size)
{
    markRange(base, size, true);
}

void PageBitmap::markUnavailable(std::uintptr_t base, std::size_t size)
{
    markRange(base, size, false);
}

std::uintptr_t PageBitmap::allocatePage(std::uintptr_t address_limit)
{
    if (address_limit < m_baseAddress)
        return 0;

    // How far are we scanning?
    auto limit = std::clamp<std::size_t>(address_limit - m_baseAddress, 0, m_length);

    // Scan the bitmap looking for free bits. We do this backwards in order to preserve pages with low addresses for
    // anything that specifically needs them.
    auto total_bits = limit / PageSize;
    auto sets = static_cast<element_t*>(m_bitmap);
    for (std::size_t i = 0; i < total_bits; ++i)
    {
        auto index = total_bits - i - 1;
        auto set_index = index / Bits;
        auto bit_index = index % Bits;

        if (sets[set_index].test(bit_index))
        {
            sets[set_index].reset(bit_index);
            return m_baseAddress + (index * PageSize); 
        }
    }

    // No page found.
    return 0;
}

std::size_t PageBitmap::getAvailableSize(std::uintptr_t address_limit) const
{
    // The accumulated available size.
    std::size_t size = 0;

    // How far are we scanning?
    auto limit = std::clamp<std::size_t>(address_limit - m_baseAddress, 0, m_length);

    // Scan the bitmap and count the number of available bits.
    std::size_t total_bits = limit / PageSize;
    auto sets = static_cast<const element_t*>(m_bitmap);
    for (std::size_t i = 0; i < total_bits; ++i)
    {
        auto set_index = i / Bits;
        auto bit_index = i % Bits;

        if (sets[set_index].test(bit_index))
            size += PageSize;
    }

    return size;
}

void PageBitmap::markRange(std::uintptr_t base, std::size_t size, bool available)
{
    // Clamp the base and size to the covered range.
    adjustBaseAndSize(base, size);
    if (size == 0)
        return;    

    // Easier to track things as a (base,end) pair than a base and a size.
    auto end = (base + size);

    if (available)
    {
        // When marking pages as available, we're conservative and only mark whole pages. The first step is to round the
        // base address up to the next page boundary.
        base = (base + PageSize - 1) & ~(PageSize - 1);

        // Similarly, the end needs to be rounded down to a page boundary.
        end = end & ~(PageSize - 1);
    }
    else
    {
        // When marking pages as unavailable, we're greedy and mark all touched pages as unavailable. The first step is
        // to round the base address down to the previous page boundary.
        base = base & ~(PageSize - 1);

        // Similarly, the end is expanded upwards to the next page boundary.
        end = (end + PageSize - 1) & ~(PageSize - 1);
    }

    // Check that we still have a range available.
    if (end <= base)
        return;

    size = end - base;

    // Calculate the first page to mark and the number of pages we'll be marking.
    auto page_number = base / PageSize;
    auto page_count = size / PageSize;

    for (std::size_t i = 0; i < page_count; ++i)
    {
        // Calculate which bitset and bit we're going to mark.
        auto n = page_number + i;
        auto bitset = n / Bits;
        auto bit = n % Bits;

        // Get the bitset and mark the bit.
        auto& set = static_cast<element_t*>(m_bitmap)[bitset];
        set.set(bit, available);
    }
}

void PageBitmap::adjustBaseAndSize(std::uintptr_t& base, std::size_t& size)
{
    // Does the range start before the range covered by this bitmap?
    if (base < m_baseAddress)
    {
        if (size > (m_baseAddress - base))
        {
            // Adjust the lower end of the range to the base.
            size -= (m_baseAddress - base);
            base = m_baseAddress;
        }
        else
        {
            // The entire range is below the base address.
            base = size = 0;
            return;
        }
    }

    // Does the range extend beyond the end of the bitmap?
    if ((base + size) > (m_baseAddress + m_length))
    {
        // Trim the size to fit.
        size = (m_baseAddress + m_length) - base;
    }
}


} // namespace System::Boot::Igniter

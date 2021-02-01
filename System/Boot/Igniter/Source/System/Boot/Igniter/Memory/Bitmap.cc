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

std::size_t PageBitmap::getAvailableSize(std::uintptr_t address_limit) const
{
    // The accumulated available size.
    std::size_t size = 0;

    // How far are we scanning?
    auto limit = std::clamp<std::size_t>(m_length, 0, address_limit - m_baseAddress);

    // Scan the bitmap and count the number of available bits.
    constexpr auto Bits = 32;
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

    if (available)
    {
        // When marking pages as available, we're conservative and only mark whole pages. The first step is to round the
        // base address up to the next page boundary.
        auto aligned_base = (base + PageSize - 1) & ~(PageSize - 1);
        auto alignment_loss = aligned_base - base;
        if (alignment_loss >= size)
            return;

        base = aligned_base;
        size -= alignment_loss;

        // Similarly, the end needs to be rounded down to a page boundary.
        auto end = base + size;
        auto aligned_end = end & ~(PageSize - 1);
        size = aligned_end - base;
    }
    else
    {
        // When marking pages as available, we're greedy and mark all touched pages as unavailable. The first step is to
        // round the base address down to the previous page boundary.
        auto aligned_base = base & ~(PageSize - 1);
        auto alignment_gain = base - aligned_base;
    
        base = aligned_base;
        size += alignment_gain;

        // Similarly, the end is expanded upwards to the next page boundary.
        auto end = base + size;
        auto aligned_end = (end + PageSize - 1) & ~(PageSize - 1);
        size = aligned_end - base;
    }

    // Calculate the first page to mark and the number of pages we'll be marking.
    auto page_number = base / PageSize;
    auto page_count = size / PageSize;

    for (std::size_t i = 0; i < page_count; ++i)
    {
        // Calculate which bitset and bit we're going to mark.
        constexpr auto Bits = 32;
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

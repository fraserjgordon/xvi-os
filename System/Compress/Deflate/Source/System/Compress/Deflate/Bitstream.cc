#include <System/Compress/Deflate/Bitstream.hh>

#include <cstring>


namespace System::Compress::Deflate
{


unsigned int Bitstream::readBit()
{
    // On EOF, arbitrarily return zeroes.
    if (eof())
        return 0;

    // If the buffer is empty, read more.
    if (m_offset > 0 && static_cast<std::size_t>(m_offset / 8) == m_buffer.size())
    {
        // If the buffer refill fails, we're at EOF.
        if (!refillBuffer(m_more))
            return 0;
    }

    // Offset of the specific bit we're going to read. If the offset is negative, we're reading from the overflow buffer
    // instead of the main buffer.
    std::size_t byteOffset = static_cast<std::size_t>((m_offset >= 0) ? (m_offset / 8) : (OverflowSize - (-m_offset / 8)));
    std::size_t bitOffset = static_cast<std::size_t>((m_offset >= 0) ? (m_offset % 8) : (8 - (-m_offset % 8)));

    // Bits are packed into bytes starting from the least-significant bit.
    auto byte = (m_offset >= 0) ? m_buffer[byteOffset] : m_overflow[byteOffset];
    auto bit = (static_cast<unsigned int>(byte) >> bitOffset) & 1;
    ++m_offset;

    return bit;
}


unsigned int Bitstream::read(unsigned int count)
{
    // Do we have enough bits remaining to complete this read?
    //
    // If not, we arbitrarily return zero.
    if (!canReadBits(count))
        return 0;

    // Accumulate bits until we've read the requested amount.
    unsigned int value = 0;
    for (unsigned int offset = 0; offset < count; ++offset)
    {
        // Read a bit. We know we got a valid one as we performed a length check earlier.
        auto bit = readBit();

        // Bits are stored in little-endian order (i.e least-significant first, most-significant last).
        value |= (bit << offset);
    }

    return value;
}


void Bitstream::byteAlign()
{
    // Round the offset upwards to the next byte boundary.
    m_offset = (m_offset + 7U) & ~7U;
}


bool Bitstream::eof() const noexcept
{
    return m_eof;
}


bool Bitstream::canReadBits(unsigned int count)
{
    // There is a maximum number of bits we can read at once, equal to the length of the overflow buffer.
    if (count > OverflowSize * 8)
        return false;

    // Check whether we can satisfy the read from the existing buffer.
    if (count <= static_cast<std::ptrdiff_t>(8 * m_buffer.size()) - m_offset)
        return true;

    // We're near the end of the buffer; perform a buffer refill.
    if (!refillBuffer(m_more))
        return false;

    // We buffered (at least) one more byte. Because we might need to read multiple bytes, we recurse here. This is
    // guaranteed to terminate because we'll either read the required number of bytes or refillBuffer will return false
    // due to no more data being available.
    return canReadBits(count);
}


std::size_t Bitstream::bitsAvailable() const noexcept
{
    return static_cast<std::size_t>(static_cast<std::ptrdiff_t>(8 * m_buffer.size()) - m_offset);
}


std::size_t Bitstream::bytesAvailable() const noexcept
{
    return bitsAvailable() / 8;
}


void Bitstream::setInput(std::span<const std::byte> buffer)
{
    // Use this as the buffer and reset the offset and EOF flag.
    m_buffer = buffer;
    m_offset = 0;
    m_eof = false;
}


void Bitstream::setMoreInputFn(MoreInputFn more)
{
    // Reset the EOF flag if we now have a non-null more method.
    if (more)
        m_eof = false;

    m_more = std::move(more);
}


bool Bitstream::refillBuffer(MoreInputFn& more)
{
    // We can't refill the buffer if we don't have a callback for it.
    if (!m_more)
        return false;

    // Copy the last few bytes of the old buffer to an overflow cache as we might still need them. We need to do this
    // before getting the new buffer as the old buffer may be invalidated.
    //
    // As part of this, we move the offset backwards (making it negative, potentially - which indicates that the next
    // few bits are to be read from the overflow until it becomes non-negative).
    m_offset -= 8 * m_buffer.size();
    if (m_buffer.size() >= OverflowSize)
    {
        // Normal case - we can just copy the last few bytes directly into the overflow.
        std::memcpy(m_overflow.data(), m_buffer.data() + m_buffer.size() - OverflowSize, OverflowSize);   
    }
    else
    {
        // Small buffer case - the buffer is smaller than the overflow so we need to keep some of the existing overflow
        // contents and fill the rest with the buffer contents.
        std::memmove(m_overflow.data(), m_overflow.data() + m_buffer.size(), OverflowSize - m_buffer.size());
        std::memcpy(m_overflow.data() + m_buffer.size(), m_buffer.data(), m_buffer.size());
    }

    // Call the callback to get a view into the next bit of the buffer.
    auto m_buffer = more();

    // We were successful if we now have a non-empty buffer.
    return !m_buffer.empty();
}


} // namespace System::Compress::Deflate

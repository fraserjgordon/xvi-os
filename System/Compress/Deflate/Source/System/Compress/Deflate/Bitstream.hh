#ifndef __SYSTEM_COMPRESS_DEFLATE_BITSTREAM_H
#define __SYSTEM_COMPRESS_DEFLATE_BITSTREAM_H


#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <span>
#include <type_traits>


namespace System::Compress::Deflate
{


class Bitstream
{
public:

    using MoreInputFn = std::function<std::span<const std::byte>()>;


    // Maximum supported input buffer size. The source of this limit is the signed bit offset stored internally.
    //
    //! @todo: convert to ptr+bit representation to raise this limit.
    static constexpr auto MaxInputSize = std::numeric_limits<std::make_signed_t<std::size_t>>::max() / 8;


    unsigned int readBit();
    unsigned int read(unsigned int count);

    void byteAlign();

    bool eof() const noexcept;

    // Returns true if a read of the given number of bits can be satisfied from the data available.
    bool canReadBits(unsigned int count);

    // Returns the number of bits available immediately (i.e without invoking the more input callback).
    std::size_t bitsAvailable() const noexcept;

    // Returns the number of bytes available immediately (i.e without invoking the more input callback).
    std::size_t bytesAvailable() const noexcept;

    // Returns a view of the next n bytes (if it is <= bytesAvailable()).
    std::span<const std::byte> bytes(std::size_t n) const;

    // Skips forward n bytes.
    void skipBytes(std::size_t n);

    // Sets the input buffer. The read pointer will be reset to the start of this buffer; this is likely to corrupt the
    // output if called part-way through decompression.
    void setInput(std::span<const std::byte>);

    // Sets the method that will be called when the input buffer is empty. This must be set if there is the possibility
    // of the input buffer emptying before the block end; it is not needed if there is some external guarantee of the
    // buffer size.
    void setMoreInputFn(MoreInputFn);


private:

    // Current input buffer.
    std::span<const std::byte>      m_buffer = {};
    std::ptrdiff_t                  m_offset = 0;
    bool                            m_eof = true;

    // Last few bytes of the previous input buffer (these are used when the end of the buffer is reached part-way
    // through a multibit read).
    static constexpr int OverflowSize = 4;
    std::array<std::byte, OverflowSize> m_overflow = {};

    // Callback that gets invoked when the input buffer is empty.
    MoreInputFn                     m_more = {};


    bool refillBuffer(MoreInputFn&);
};


} // namespace System::Compress::Deflate


#endif /* ifndef __SYSTEM_COMPRESS_DEFLATE_BITSTREAM_H */

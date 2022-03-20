#ifndef __SYSTEM_COMPRESS_DEFLATE_LZ77_H
#define __SYSTEM_COMPRESS_DEFLATE_LZ77_H


#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>


namespace System::Compress::Deflate
{


// Error states for the LZ77 decoder.
enum class LZ77Error
{
    None            = 0,    // No error.
    InvalidLength,          // Length of backreference was invalid.
    InvalidDistance,        // Distance of backreference was invalid.
    OutputOverflow,         // Unable to write any more output.
};


enum class Symbol : std::uint16_t
{
    // The values 0-255 (inclusive) are literals and aren't given names here (it isn't useful).

    // End-of-block marker.
    End = 256,

    // Length codes.
    FirstLengthCode = 257,
    LastLengthCode = 285,
};

constexpr auto operator<=>(Symbol a, Symbol b)
{
    return static_cast<std::uint16_t>(a) <=> static_cast<std::uint16_t>(b);
}


enum class DistanceSymbol : std::uint8_t {};


class LZ77Decoder
{
public:

    // Function for the decoder to call when the output buffer has insufficient space remaining.
    using FlushBufferFn = std::function<void(std::span<std::byte> rangeToFlush)>;

    // Maxima for the LZ77 backreferencers.
    static constexpr std::size_t MaxBackrefDistance = 32768;
    static constexpr std::size_t MaxBackrefLength = 258;

    // The output buffer needs to be large enough to support a backreference of the maximum length (258) at the maximum
    // backwards distance (32768).
    static constexpr std::size_t MinOutputBufferSize = MaxBackrefDistance + MaxBackrefLength;


    constexpr LZ77Decoder() = default;
    LZ77Decoder(const LZ77Decoder&) = default;
    LZ77Decoder(LZ77Decoder&&) = default;

    ~LZ77Decoder() = default;

    LZ77Decoder& operator=(const LZ77Decoder&) = default;
    LZ77Decoder& operator=(LZ77Decoder&&) = default;


    // Sets the method that will be called when the output buffer is full. This must be set if there is the possibility
    // of the output buffer filling; it is not needed if there is some external guarantee of the buffer size.
    void setOutputFlushFn(FlushBufferFn);

    // Instructs the decoder to output the given literal byte.
    void processLiteral(std::byte literal);

    // Instructs the decoder to output the given literal bytes.
    void processBytes(std::span<const std::byte> literal);

    // Instructs the decoder to act on the given length/distance pair.
    void processBackref(unsigned int length, unsigned int distance);

    // Finishes the current block and flushes any buffered output.
    void finish();


    static bool isLengthCode(Symbol) noexcept;

    static unsigned int lengthCodeExtraBits(Symbol) noexcept;

    static unsigned int lengthCodeBaseLength(Symbol) noexcept;

    static unsigned int lengthCodeLength(Symbol, unsigned int extra) noexcept;

    static unsigned int distanceCodeExtraBits(DistanceSymbol) noexcept;

    static unsigned int distanceCodeBaseDistance(DistanceSymbol) noexcept;

    static unsigned int distanceCodeDistance(DistanceSymbol, unsigned int extra) noexcept;


private:

    // Output buffer. This is both written (as the input is decompressed) and read (in order to copy backreferences).
    std::span<std::byte>        m_output = {};
    std::size_t                 m_offset = 0;

    // Callback for flushing the output buffer.
    FlushBufferFn               m_flushOutput = {};


    // Flushes any output that is no longer needed (i.e anything that is no longer reachable via a backreference).
    void flush();
};


} // namespace System::Compress::Deflate


#endif /* ifndef __SYSTEM_COMPRESS_DEFLATE_LZ77_H */

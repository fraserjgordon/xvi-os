#include <System/Unicode/UTF/UTF8Codec.hh>

#include <gtest/gtest.h>


using namespace std;
using namespace System::Unicode::UTF;


TEST(UTF8CodecPrimitivesTest, DecodeSeqLengthValid)
{
    // Validate that all ASCII characters are valid and have length one.
    for (uint8_t c = 0x00; c < 0x80; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c), pair(true, size_t(1)));

    // Validate all bytes that start a two-byte sequence.
    for (uint8_t c = 0xC0; c < 0xE0; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c), pair(true, size_t(2)));
    
    // Validate all bytes that start a three-byte sequence.
    for (uint8_t c = 0xE0; c < 0xF0; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c), pair(true, size_t(3)));

    // Validate all bytes that start a four-byte sequence.
    for (uint8_t c = 0xF0; c < 0xF8; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c), pair(true, size_t(4)));
}

TEST(UTF8CodecPrimitivesTest, DecodeSeqLengthInvalid)
{
    // Check that no continuation characters are reported as having a valid sequence length.
    for (uint8_t c = 0x80; c < 0xC0; ++c)
        ASSERT_FALSE(UTF8Codec::decodeSeqLength(c).first);

    // Check that 5- and 6-byte sequence starters are invalid.
    for (uint8_t c = 0xF8; c < 0xFE; ++c)
        ASSERT_FALSE(UTF8Codec::decodeSeqLength(c).first);

    // Check that bytes that are never used in UTF-8 are not valid sequence starters.
    ASSERT_FALSE(UTF8Codec::decodeSeqLength(0xFE).first);
    ASSERT_FALSE(UTF8Codec::decodeSeqLength(0xFF).first);    
}

TEST(UTF8CodecPrimitivesTest, DecodeSeqLengthLong)
{
    // Check that the 5-byte sequence starters are recognised with the appropriate variation flag.
    for (uint8_t c = 0xF8; c < 0xFC; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c, UTF8Variant::AllowNonUnicode), pair(true, size_t(5)));

    // Check that the 6-byte sequence starters are recognised with the appropriate variation flag.
    for (uint8_t c = 0xFC; c < 0xFE; ++c)
        ASSERT_EQ(UTF8Codec::decodeSeqLength(c, UTF8Variant::AllowNonUnicode), pair(true, size_t(6)));
}

TEST(UTF8CodecPrimitivesTest, IsContinuationByte)
{
    // Check that all continuation bytes are correctly recognised as such.
    for (uint8_t c = 0x80; c < 0xC0; ++c)
        ASSERT_TRUE(UTF8Codec::isContinuationByte(c));

    // Similarly, check that it is not true for all non-continuation bytes.
    for (uint8_t c = 0x00; c < 0x80; ++c)
        ASSERT_FALSE(UTF8Codec::isContinuationByte(c));
    for (uint8_t c = 0xC0; c > 0; ++c)
        ASSERT_FALSE(UTF8Codec::isContinuationByte(c));
}

TEST(UTF8CodecPrimitivesTest, EncodedSize)
{
    // Check the encoded size of all Unicode codepoints.
    for (char32_t c = 0; c < 0x80; ++c)
        ASSERT_EQ(UTF8Codec::encodedSize(c), 1U);
    for (char32_t c = 0x80; c < 0x800; ++c)
        ASSERT_EQ(UTF8Codec::encodedSize(c), 2U);
    for (char32_t c = 0x800; c < 0x10000; ++c)
        ASSERT_EQ(UTF8Codec::encodedSize(c), 3U);
    for (char32_t c = 0x10000; c <= 0x10FFFF; ++c)
        ASSERT_EQ(UTF8Codec::encodedSize(c), 4U);
}

TEST(UTF8CodecPrimitivesTest, DecodeOneUncheckedBasic)
{
    // Just a small sample of test codepoints.
    ASSERT_EQ(UTF8Codec::decodeOneUnchecked(u8"$"), pair(U'$', size_t(1)));
    ASSERT_EQ(UTF8Codec::decodeOneUnchecked(u8"¢"), pair(U'¢', size_t(2)));
    ASSERT_EQ(UTF8Codec::decodeOneUnchecked(u8"€"), pair(U'€', size_t(3)));
    ASSERT_EQ(UTF8Codec::decodeOneUnchecked(u8"😂"), pair(U'😂', size_t(4)));
}

TEST(UTF8CodecPrimitivesTest, DecodeOneBasic)
{
    // Just a small sample of test codepoints.
    ASSERT_EQ(UTF8Codec::decodeOne(u8"$", 1), pair(U'$', size_t(1)));
    ASSERT_EQ(UTF8Codec::decodeOne(u8"¢", 2), pair(U'¢', size_t(2)));
    ASSERT_EQ(UTF8Codec::decodeOne(u8"€", 3), pair(U'€', size_t(3)));
    ASSERT_EQ(UTF8Codec::decodeOne(u8"😂", 4), pair(U'😂', size_t(4)));
}

TEST(UTF8CodecPrimitivesTest, IsValidOverlongNuls)
{
    // Note that all of these sequences decode to NUL (U+0000).
    ASSERT_TRUE(UTF8Codec::isValid(u8"", 1));
    ASSERT_FALSE(UTF8Codec::isValid(u8"\xC0\x80", 2));
    ASSERT_TRUE(UTF8Codec::isValid(u8"\xC0\x80", 2, UTF8Variant::OverlongNuls));
    ASSERT_FALSE(UTF8Codec::isValid(u8"\xE0\x80\x80", 3, UTF8Variant::OverlongNuls));
    ASSERT_TRUE(UTF8Codec::isValid(u8"\xE0\x80\x80", 3, UTF8Variant::AllowOverlong));
}

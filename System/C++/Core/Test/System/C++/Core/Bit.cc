#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Bit.hh>

#include <cstdint>


using namespace __XVI_STD_CORE_NS;


TEST(Bit, Endian)
{
    EXPECT_NE(endian::little, endian::big);
    EXPECT(endian::native == endian::little || endian::native == endian::big);

    union
    {
        uint8_t u8[4] = {1, 2, 3, 4};
        uint32_t u32;
    };

    if constexpr (endian::native == endian::big)
        EXPECT_EQ(u32, 0x01020304);
    else
        EXPECT_EQ(u32, 0x04030201);
}


TEST(Bit, BitCast)
{
    constexpr std::uint8_t arr[4] = {1, 2, 3, 4};

    constexpr auto val = bit_cast<std::uint32_t>(arr);

    if constexpr (endian::native == endian::big)
        EXPECT_EQ(val, 0x01020304);
    else
        EXPECT_EQ(val, 0x04030201);
}

TEST(Bit, ByteSwap)
{
    EXPECT_EQ(byteswap(uint8_t(0x5a)), 0x5a);
    EXPECT_EQ(byteswap(uint16_t(0xaa55)), 0x55aa);
    EXPECT_EQ(byteswap(UINT32_C(0x01020304)), 0x04030201);
    EXPECT_EQ(byteswap(UINT64_C(0x0102030405060708)), 0x0807060504030201);
}

TEST(Bit, HasSingleBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        auto x = UINT64_C(1) << i;

        EXPECT(has_single_bit(x));

        EXPECT(i == 0 || !has_single_bit(x + 1));
        EXPECT(i == 0 || !has_single_bit(x + (x >> 1)));
    }
}

TEST(Bit, BitCeil)
{
    for (unsigned i = 0; i < 63; ++i)
    {
        auto x = UINT64_C(1) << i;

        EXPECT_EQ(bit_ceil(x), x);
        EXPECT_EQ(bit_ceil(x + 1), x << 1);

        if (i > 0)
            EXPECT_EQ(bit_ceil(x + (x - 1)), x << 1);
    }

    auto x = UINT64_C(1) << 63;
    EXPECT_EQ(bit_ceil(x), x);

    EXPECT_EQ(bit_ceil(0U), 1);
}

TEST(Bit, BitFloor)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        auto x = UINT64_C(1) << i;

        EXPECT_EQ(bit_floor(x), x);

        if (i > 0)
            EXPECT_EQ(bit_floor(x + 1), x);

        EXPECT_EQ(bit_floor(x + (x - 1)), x);
    }

    EXPECT_EQ(bit_floor(0U), 0);
}

TEST(Bit, BitWidth)
{
    for (unsigned i = 0; i < 63; ++i)
    {
        auto x = UINT64_C(1) << i;

        EXPECT_EQ(bit_width(x), i + 1);

        if (i > 0)
            EXPECT_EQ(bit_width(x + 1), i + 1);

        EXPECT_EQ(bit_width(x + (x - 1)), i + 1);
    }

    EXPECT_EQ(bit_width(0U), 0);
}

TEST(Bit, Rotl)
{
    EXPECT_EQ(rotl(uint8_t(0b11001001), 3), 0b01001110);
    EXPECT_EQ(rotl(uint16_t(0x1234), 4), 0x2341);
    EXPECT_EQ(rotl(UINT32_C(0x12345678), 20), 0x67812345);
    EXPECT_EQ(rotl(UINT64_C(0x123456789abcdef0), 60), 0x0123456789abcdef);

    EXPECT_EQ(rotl(UINT32_C(0x12345678), -4), 0x81234567);
}

TEST(Bit, Rotr)
{
    EXPECT_EQ(rotr(uint8_t(0b11001001), 3), 0b00111001);
    EXPECT_EQ(rotr(uint16_t(0x1234), 4), 0x4123);
    EXPECT_EQ(rotr(UINT32_C(0x12345678), 20), 0x45678123);
    EXPECT_EQ(rotr(UINT64_C(0x123456789abcdef0), 60), 0x23456789abcdef01);

    EXPECT_EQ(rotr(UINT32_C(0x12345678), -4), 0x23456781);
}

TEST(Bit, CountlZero)
{
    EXPECT_EQ(countl_zero(uint8_t(0)), 8U);
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(countl_zero(uint8_t(1U << i)), 7 - i);

    EXPECT_EQ(countl_zero(uint16_t(0)), 16U);
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(countl_zero(uint16_t(1U << i)), 15 - i);

    EXPECT_EQ(countl_zero(UINT32_C(0)), 32U);
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(countl_zero(UINT32_C(1) << i), 31 - i);

    EXPECT_EQ(countl_zero(UINT64_C(0)), 64U);
    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(countl_zero(UINT64_C(1) << i), 63 - i);
}

TEST(Bit, CountlOne)
{
    EXPECT_EQ(countl_one(uint8_t(~0)), 8U);
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(countl_one(uint8_t(~(1U << i))), 7 - i);

    EXPECT_EQ(countl_one(uint16_t(~0)), 16U);
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(countl_one(uint16_t(~(1U << i))), 15 - i);

    EXPECT_EQ(countl_one(~UINT32_C(0)), 32U);
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(countl_one(~(UINT32_C(1) << i)), 31 - i);

    EXPECT_EQ(countl_one(~UINT64_C(0)), 64U);
    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(countl_one(~(UINT64_C(1) << i)), 63 - i);
}

TEST(Bit, CountrZero)
{
    EXPECT_EQ(countr_zero(uint8_t(0)), 8U);
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(countr_zero(uint8_t(1U << i)), i);

    EXPECT_EQ(countr_zero(uint16_t(0)), 16U);
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(countr_zero(uint16_t(1U << i)), i);

    EXPECT_EQ(countr_zero(UINT32_C(0)), 32U);
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(countr_zero(UINT32_C(1) << i), i);

    EXPECT_EQ(countr_zero(UINT64_C(0)), 64U);
    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(countr_zero(UINT64_C(1) << i), i);
}

TEST(Bit, CountrOne)
{
    EXPECT_EQ(countr_one(uint8_t(~0)), 8U);
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(countr_one(uint8_t(~(1U << i))), i);

    EXPECT_EQ(countr_one(uint16_t(~0)), 16U);
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(countr_one(uint16_t(~(1U << i))), i);

    EXPECT_EQ(countr_one(~UINT32_C(0)), 32U);
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(countr_one(~(UINT32_C(1) << i)), i);

    EXPECT_EQ(countr_one(~UINT64_C(0)), 64U);
    for (int i = 0; i < 64; ++i)
        EXPECT_EQ(countr_one(~(UINT64_C(1) << i)), i);
}

TEST(Bit, Popcount)
{
    EXPECT_EQ(popcount(UINT32_C(0)), 0);
    EXPECT_EQ(popcount(~UINT32_C(0)), 32);
    EXPECT_EQ(popcount(0x5555aaaaU), 16);
    EXPECT_EQ(popcount(0x12488421U), 8);
}

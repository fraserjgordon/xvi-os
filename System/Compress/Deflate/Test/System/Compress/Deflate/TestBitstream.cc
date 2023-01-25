#include <Tools/Development/Test/Test.hh>

#include <System/Compress/Deflate/Bitstream.hh>

#include <cstring>


using namespace System::Compress::Deflate;


TEST(Bitstream, ReadSingleBits)
{
    const unsigned char input[] = { 0x55, 0xAA, 0x00, 0xFF };
    unsigned char output [4] = {0, 0, 0, 0};

    Bitstream bs;
    bs.setInput(std::span{reinterpret_cast<const std::byte*>(input), sizeof(input)});

    EXPECT_EQ(bs.bitsAvailable(), 32);

    for (unsigned i = 0; i < 32; ++i)
    {
        auto bit = bs.readBit();

        EXPECT_EQ(bs.bitsAvailable(), 32u - i - 1);

        output[i / 8] |= (bit << (i % 8));
    }

    EXPECT_EQ(std::memcmp(input, output, 4), 0);
}

TEST(Bitstream, ReadBytes)
{
    const unsigned char input[] = { 0x55, 0xAA, 0x00, 0xFF };
    unsigned char output [4] = {0, 0, 0, 0};

    Bitstream bs;
    bs.setInput(std::span{reinterpret_cast<const std::byte*>(input), sizeof(input)});

    EXPECT_EQ(bs.bytesAvailable(), 4);

    for (unsigned i = 0; i < 4; ++i)
    {
        EXPECT(bs.canReadBits(8));
        auto byte = bs.read(8);

        EXPECT_EQ(bs.bytesAvailable(), 4u - i - 1);

        output[i] = byte;
    }

    EXPECT_EQ(std::memcmp(input, output, 4), 0);
}

#include <Tools/Development/Test/Test.hh>

#include <System/Compress/Deflate/Huffman.hh>

#include <string_view>

#include <System/Compress/Deflate/TestUtils.hh>


using namespace System::Compress::Deflate;


static constexpr auto staticTreeLookup(std::string_view bits)
{
    auto tree = HuffmanTree::makeStaticTree();
    auto node = tree.root();
    for (auto b : bits)
    {
        if (b == '0')
            node = node->zero();
        else
            node = node->one();
    }

    return static_cast<std::uint16_t>(node->value());
}


TEST(Huffman, TreeLookup)
{
    EXPECT_EQ(staticTreeLookup("00110000"), 0);
    EXPECT_EQ(staticTreeLookup("10111111"), 143);
    EXPECT_EQ(staticTreeLookup("110010000"), 144);
    EXPECT_EQ(staticTreeLookup("111111111"), 255);
    EXPECT_EQ(staticTreeLookup("0000000"), 256);
    EXPECT_EQ(staticTreeLookup("0010111"), 279);
    EXPECT_EQ(staticTreeLookup("11000000"), 280);
    EXPECT_EQ(staticTreeLookup("11000111"), 287);
}

TEST(Huffman, StaticTreeHello)
{
    

    auto tree = HuffmanTree::makeStaticTree();
}

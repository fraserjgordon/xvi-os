#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>

#include <limits>


TEST(Strnlen, ExactLength)
{
    EXPECT_EQ(__strnlen("", 1), 0);
    EXPECT_EQ(__strnlen("Hello, World!", 14), 13);
    EXPECT_EQ(__strnlen("\0", 1), 0);
}

TEST(Strnlen, SizeMax)
{
    constexpr auto SizeMax = std::numeric_limits<std::size_t>::max();
    EXPECT_EQ(__strnlen("", SizeMax), 0);
    EXPECT_EQ(__strnlen("Hello, World!", SizeMax), 13);
}

TEST(Strnlen, Partial)
{
    EXPECT_EQ(__strnlen("Hello, World!", 5), 5);
    EXPECT_EQ(__strnlen("Hi", 0), 0);
}

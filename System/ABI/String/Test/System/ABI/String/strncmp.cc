#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>

#include <limits>


TEST(Strncmp, PartialCompare)
{
    EXPECT_EQ(__strncmp("Hello, World!", "Hello", 5), 0);
    EXPECT_LT(__strncmp("aaa", "abc", 2), 0);
    EXPECT_GT(__strncmp("ba", "aa", 1), 0);
}

TEST(Strncmp, SizeMax)
{
    constexpr auto SizeMax = std::numeric_limits<std::size_t>::max();
    EXPECT_EQ(__strncmp("Hello", "Hello", SizeMax), 0);
    EXPECT_LT(__strncmp("aaa", "abc", SizeMax), 0);
}

TEST(Strncmp, UnequalLength)
{
    EXPECT_LT(__strncmp("aa", "aaa", 3), 0);
    EXPECT_GT(__strncmp("aaa", "aa", 3), 0);
}

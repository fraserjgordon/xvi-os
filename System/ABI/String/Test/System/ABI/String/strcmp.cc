#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>

#include <limits>


TEST(Strcmp, EqualLength)
{
    EXPECT_EQ(__strcmp("aaa", "aaa"), 0);
    EXPECT_LT(__strcmp("aaa", "aba"), 0);
    EXPECT_GT(__strcmp("aab", "aaa"), 0);
}


TEST(Strcmp, UnequalLength)
{
    EXPECT_LT(__strcmp("aa", "aaa"), 0);
    EXPECT_GT(__strcmp("aaa", "aa"), 0);
}

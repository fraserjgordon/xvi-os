#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>

#include <limits>


TEST(Memcmp, Compare)
{
    EXPECT_EQ(__memcmp("aaa", "aaa", 3), 0);
    EXPECT_LT(__memcmp("aaa", "aba", 3), 0);
    EXPECT_GT(__memcmp("aab", "aaa", 3), 0);
}

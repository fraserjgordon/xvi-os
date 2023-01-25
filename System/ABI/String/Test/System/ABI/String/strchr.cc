#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, needle, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__strchr(str, needle), str+offset); \
    }

TEST(Strchr, NotFound)
{
    EXPECT_EQ(__strchr("Hello", 'h'), nullptr);
    EXPECT_EQ(__strchr("Hi", '\0'), nullptr);
    EXPECT_EQ(__strchr("", '\0'), nullptr);
    EXPECT_EQ(__strchr("", '!'), nullptr);
}

TEST(Strchr, FindFirst)
{
    EXPECT_AT_OFFSET("r", 'r', 0);
    EXPECT_AT_OFFSET("rr", 'r', 0);
    EXPECT_AT_OFFSET("rrr", 'r', 0);
    EXPECT_AT_OFFSET("rrrw", 'r', 0);
    EXPECT_AT_OFFSET("wrrw", 'r', 1);
}

TEST(Strchr, AtEnd)
{
    EXPECT_AT_OFFSET("wwwwwwr", 'r', 6);
}

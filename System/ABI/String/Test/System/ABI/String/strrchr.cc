#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, needle, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__strrchr(str, needle), str+offset); \
    }

TEST(Strrchr, NotFound)
{
    EXPECT_EQ(__strrchr("Hello", 'h'), nullptr);
    EXPECT_EQ(__strrchr("Hi", '\0'), nullptr);
    EXPECT_EQ(__strrchr("", '\0'), nullptr);
    EXPECT_EQ(__strrchr("", '!'), nullptr);
}

TEST(Strrchr, FindLast)
{
    EXPECT_AT_OFFSET("r", 'r', 0);
    EXPECT_AT_OFFSET("rr", 'r', 1);
    EXPECT_AT_OFFSET("rrr", 'r', 2);
    EXPECT_AT_OFFSET("rrrw", 'r', 2);
    EXPECT_AT_OFFSET("wrrw", 'r', 2);
}

TEST(Strrchr, AtStart)
{
    EXPECT_AT_OFFSET("rwwwwww", 'r', 0);
}

#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, needle, size, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__memrchr(str, needle, size), str+offset); \
    }

TEST(Memrchr, NotFound)
{
    EXPECT_EQ(__memrchr("Hello", 'h', 6), nullptr);
    EXPECT_EQ(__memrchr("Hi", '\0', 2), nullptr);
    EXPECT_EQ(__memrchr("", '\0', 0), nullptr);
    EXPECT_EQ(__memrchr("", '!', 1), nullptr);
}

TEST(Memrchr, Nullptr)
{
    EXPECT_EQ(__memrchr(nullptr, 0, 0), nullptr);
}

TEST(Memrchr, FindLast)
{
    EXPECT_AT_OFFSET("r", 'r', 2, 0);
    EXPECT_AT_OFFSET("rr", 'r', 3, 1);
    EXPECT_AT_OFFSET("rrr", 'r', 4, 2);
    EXPECT_AT_OFFSET("rrrw", 'r', 5, 2);
    EXPECT_AT_OFFSET("wrrw", 'r', 5, 2);
}

TEST(Memrchr, AtStart)
{
    EXPECT_AT_OFFSET("rwwwwww", 'r', 7, 0);
}

TEST(Memrchr, Nuls)
{
    EXPECT_AT_OFFSET("foo\0bar", '\0', 7, 3);
    EXPECT_AT_OFFSET("foo\0bar", 'b', 8, 4);
}

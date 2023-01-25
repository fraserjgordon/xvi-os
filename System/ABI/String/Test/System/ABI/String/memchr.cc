#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, needle, size, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__memchr(str, needle, size), str+offset); \
    }

TEST(Memchr, NotFound)
{
    EXPECT_EQ(__memchr("Hello", 'h', 6), nullptr);
    EXPECT_EQ(__memchr("Hi", '\0', 2), nullptr);
    EXPECT_EQ(__memchr("", '\0', 0), nullptr);
    EXPECT_EQ(__memchr("", '!', 1), nullptr);
}

TEST(Memchr, Nullptr)
{
    EXPECT_EQ(__memchr(nullptr, 0, 0), nullptr);
}

TEST(Memchr, FindFirst)
{
    EXPECT_AT_OFFSET("r", 'r', 2, 0);
    EXPECT_AT_OFFSET("rr", 'r', 3, 0);
    EXPECT_AT_OFFSET("rrr", 'r', 4, 0);
    EXPECT_AT_OFFSET("rrrw", 'r', 5, 0);
    EXPECT_AT_OFFSET("wrrw", 'r', 5, 1);
}

TEST(Memchr, AtEnd)
{
    EXPECT_AT_OFFSET("wwwwwwr", 'r', 7, 6);
}

TEST(Memchr, Nuls)
{
    EXPECT_AT_OFFSET("foo\0bar", '\0', 7, 3);
    EXPECT_AT_OFFSET("foo\0bar", 'b', 8, 4);
}

#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, needle, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__strstr(str, needle), str+offset); \
    }


static constexpr auto LongHaystack = R"(
This is a rather long string to be used for testing strstr with a large haystack
or needle (or both). None of this is meaningful; it is simply rambling in order
to fill an arbitrary "long enough" quota.

The phrase "rather long string" is repeated to ensure we find the right one.

As some of the tests hard-code expected offsets, please don't change this text
without adjusting those offsets accordingly.
)";


TEST(Strstr, NotFound)
{
    EXPECT_EQ(__strstr("Hello, World!", "hi"), nullptr);
    EXPECT_EQ(__strstr("Hi", "hi"), nullptr);
    EXPECT_EQ(__strstr("Hell", "Hello"), nullptr);
}

TEST(Strstr, NotFoundLong)
{
    EXPECT_EQ(__strstr(LongHaystack, "these words don't appear"), nullptr);
}

TEST(Strstr, AtStart)
{
    EXPECT_AT_OFFSET("Hello, World!", "Hello", 0);
    EXPECT_AT_OFFSET("Hello Hello", "Hello", 0);
    EXPECT_AT_OFFSET("Hello", "H", 0);
    EXPECT_AT_OFFSET("Hello, World!", "", 0);
}

TEST(Strstr, AtStartLong)
{
    EXPECT_AT_OFFSET(LongHaystack, "\nThis is a rather long string", 0);
}

TEST(Strstr, Found)
{
    EXPECT_AT_OFFSET("Hello, World!", "World", 7)
}

TEST(Strstr, FoundLong)
{
    EXPECT_AT_OFFSET(LongHaystack, "rather long string", 11);
}

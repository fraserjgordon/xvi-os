#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


#define EXPECT_AT_OFFSET(haystack, hsize, needle, nsize, offset) \
    { \
        auto* str = haystack; \
        EXPECT_EQ(__memmem(str, hsize, needle, nsize), str+offset); \
    }


static constexpr auto LongHaystack = R"(
This is a rather long string to be used for testing strstr with a large haystack
or needle (or both). None of this is meaningful; it is simply rambling in order
to fill an arbitrary "long enough" quota.

The phrase "rather long string" is repeated to ensure we find the right one.
)"
"With some \0embedded\0nuls\0too, of course"
R"(
As some of the tests hard-code expected offsets, please don't change this text
without adjusting those offsets accordingly.
)";


TEST(Memmem, NotFound)
{
    EXPECT_EQ(__memmem("Hello, World!", 13, "hi", 2), nullptr);
    EXPECT_EQ(__memmem("Hi", 2, "hi", 2), nullptr);
    EXPECT_EQ(__memmem("Hell", 4, "Hello", 5), nullptr);
}

TEST(Memmem, NotFoundLong)
{
    EXPECT_EQ(__memmem(LongHaystack, __strlen(LongHaystack), "these words don't appear", 24), nullptr);
}

TEST(Memmem, AtStart)
{
    EXPECT_AT_OFFSET("Hello, World!", 13, "Hello", 5, 0);
    EXPECT_AT_OFFSET("Hello Hello", 11, "Hello", 5, 0);
    EXPECT_AT_OFFSET("Hello", 5, "H", 1, 0);
    EXPECT_AT_OFFSET("Hello, World!", 13, "", 0, 0);
}

TEST(Memmem, AtStartLong)
{
    EXPECT_AT_OFFSET(LongHaystack, __strlen(LongHaystack), "\nThis is a rather long string", 29, 0);
}

TEST(Memmem, Found)
{
    EXPECT_AT_OFFSET("Hello, World!", 13, "World", 5, 7)
}

TEST(Memmem, FoundLong)
{
    EXPECT_AT_OFFSET(LongHaystack, __strlen(LongHaystack), "rather long string", 18, 11);
}

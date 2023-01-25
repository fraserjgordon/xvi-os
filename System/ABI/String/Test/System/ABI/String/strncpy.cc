#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strncpy, NulPadding)
{
    auto Expected = "Hello, World!\0\0";
    char buffer[16];

    __strncpy(buffer, "Hello, World!", sizeof(buffer));

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
}

TEST(Strncpy, NoTerminator)
{
    auto Expected = "Hello, World";
    char buffer[12];

    __strncpy(buffer, "Hello, World!", sizeof(buffer));

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), sizeof(buffer));
}

TEST(Strncpy, Truncation)
{
    auto Expected = "Hello,";
    char buffer[6];

    __strncpy(buffer, "Hello, World!", sizeof(buffer));

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
}

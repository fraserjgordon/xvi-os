#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Stpncpy, NulPadding)
{
    auto Expected = "Hello, World!\0\0";
    char buffer[16];

    EXPECT_EQ(__stpncpy(buffer, "Hello, World!", sizeof(buffer)), buffer + 13);

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
}

TEST(Stpncpy, NoTerminator)
{
    auto Expected = "Hello, World";
    char buffer[12];

    EXPECT_EQ(__stpncpy(buffer, "Hello, World!", sizeof(buffer)), buffer + sizeof(buffer));

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), sizeof(buffer));
}

TEST(Stpncpy, Truncation)
{
    auto Expected = "Hello,";
    char buffer[6];

    EXPECT_EQ(__stpncpy(buffer, "Hello, World!", sizeof(buffer)), buffer + sizeof(buffer));

    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
}

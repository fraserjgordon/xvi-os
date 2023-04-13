#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strncat, NulPadding)
{
    auto Expected = "Hi there\0\0\0\0\0\0\0";
    char buffer[16] = {'H', 'i', '\0'};

    __strncat(buffer, " there", sizeof(buffer) - 1);
    EXPECT_EQ(__memcmp(buffer, Expected, sizeof(buffer)), 0);
}

TEST(Strncat, NulTerminator)
{
    char buffer[16] = {'H', 'i', '\0'};

    __strncat(buffer, " there", sizeof(buffer) - 1);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 8);

    __strncat(buffer, ": this is too long", sizeof(buffer) - 9);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), sizeof(buffer) - 1);
}

TEST(Strncat, Concat)
{
    char buffer[16] = { '\0' };

    __strncat(buffer, "Hello", sizeof(buffer) - 1);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 5);
    EXPECT_EQ(__strcmp(buffer, "Hello"), 0);

    __strncat(buffer, ", World!", sizeof(buffer) - 6);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(__strcmp(buffer, "Hello, World!"), 0);
}

#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strlcat, NulTerminator)
{
    char buffer[16] = {'H', 'i', '\0'};

    __strlcat(buffer, " there", sizeof(buffer));
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 8);

    __strlcat(buffer, ": this is too long", sizeof(buffer));
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), sizeof(buffer) - 1);
}

TEST(Strlcat, Concat)
{
    char buffer[16] = { '\0' };

    __strlcat(buffer, "Hello", sizeof(buffer));
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 5);
    EXPECT_EQ(__strcmp(buffer, "Hello"), 0);

    __strlcat(buffer, ", World!", sizeof(buffer));
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(__strcmp(buffer, "Hello, World!"), 0);
}

TEST(Strlcat, Retval)
{
    char buffer[16] = { '\0' };

    EXPECT_EQ(__strlcat(buffer, "Hello, World!", sizeof(buffer)), 13);
    EXPECT_GT(__strlcat(buffer, ": this is too long", sizeof(buffer)), sizeof(buffer));
}

TEST(Strlcat, DestFull)
{
    // Corner cases when dest is full & unterminated.
    char buffer[5] = {'H', 'e', 'l', 'l', 'o'};

    EXPECT_EQ(__strlcat(buffer, ", World!", sizeof(buffer)), sizeof(buffer));
}

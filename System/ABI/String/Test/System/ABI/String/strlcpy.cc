#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strlcpy, Termination)
{
    char buffer[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    __strlcpy(buffer, "Hello, World!", sizeof(buffer));
    EXPECT_EQ(__memcmp(buffer, "Hello, World!", 13), 0);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(buffer[14], 14);
    EXPECT_EQ(buffer[15], 15);

    __strlcpy(buffer, "This is too long for the buffer", sizeof(buffer));
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), sizeof(buffer) - 1);
}

TEST(Strlcpy, Retval)
{
    char buffer[16];

    EXPECT_EQ(__strlcpy(buffer, "Hello, World!", sizeof(buffer)), 13);
    EXPECT_GT(__strlcpy(buffer, "This is too long for the buffer", sizeof(buffer)), sizeof(buffer));
}

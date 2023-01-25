#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Stpcpy, Copy)
{
    char buffer[16];

    EXPECT_EQ(__stpcpy(buffer, "Hello, World!"), buffer + 13);
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(__memcmp(buffer, "Hello, World!", 14), 0);
}

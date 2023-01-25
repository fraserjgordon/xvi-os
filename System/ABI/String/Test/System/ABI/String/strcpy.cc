#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strcpy, Copy)
{
    char buffer[16];

    __strcpy(buffer, "Hello, World!");
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(__memcmp(buffer, "Hello, World!", 14), 0);
}

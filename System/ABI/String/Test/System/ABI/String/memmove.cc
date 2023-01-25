#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Memmove, Overlap)
{
    char buffer[16];

    __memmove(buffer, "Hello, World!", 14);
    __memmove(buffer, buffer + 7, 5);
    EXPECT_EQ(__memcmp(buffer, "World, World!", 14), 0);

    __memmove(buffer, "Hello, World!", 14);
    __memmove(buffer + 7, buffer, 5);
    EXPECT_EQ(__memcmp(buffer, "Hello, Hello!", 14), 0);
}

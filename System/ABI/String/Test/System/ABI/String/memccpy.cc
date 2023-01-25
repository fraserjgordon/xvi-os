#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Memccpy, Nul)
{
    char buffer[16];

    __memset(buffer, -1, sizeof(buffer));
    __memccpy(buffer, "Hello, World!", 0, 14);

    EXPECT_EQ(__memcmp(buffer, "Hello, World!", 13), 0);
    EXPECT_EQ(buffer[13], char(-1));
}

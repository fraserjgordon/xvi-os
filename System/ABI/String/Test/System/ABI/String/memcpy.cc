#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Memcpy, Copy)
{
    char buffer[16];

    __memset(buffer, -1, sizeof(buffer));
    __memcpy(buffer, "Hello, World!", 14);    

    EXPECT_EQ(__memcmp(buffer, "Hello, World!\0\xff\xff", sizeof(buffer)), 0);
}

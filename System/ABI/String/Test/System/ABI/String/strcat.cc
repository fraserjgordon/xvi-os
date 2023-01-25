#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strcat, Concat)
{
    char buffer[16] = { '\0' };

    __strcat(buffer, "Hello");
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 5);
    EXPECT_EQ(__strcmp(buffer, "Hello"), 0);

    __strcat(buffer, ", World!");
    EXPECT_EQ(__strnlen(buffer, sizeof(buffer)), 13);
    EXPECT_EQ(__strcmp(buffer, "Hello, World!"), 0);
}

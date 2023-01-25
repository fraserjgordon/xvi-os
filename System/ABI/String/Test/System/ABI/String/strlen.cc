#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Strlen, Length)
{
    EXPECT_EQ(__strlen(""), 0);
    EXPECT_EQ(__strlen("Hello, World!"), 13);
    EXPECT_EQ(__strlen("\0"), 0);
}

#include <Tools/Development/Test/Test.hh>

#include <System/ABI/String/String.hh>


TEST(Memset, Set)
{
    char buffer[16];

    __memset(buffer, 0, sizeof(buffer));
    EXPECT_EQ(buffer[0], 0);
    EXPECT_EQ(buffer[7], 0);
    EXPECT_EQ(buffer[15], 0);

    __memset(buffer, 0xff, sizeof(buffer));
    EXPECT_EQ(buffer[0], -1);
    EXPECT_EQ(buffer[3], -1);
    EXPECT_EQ(buffer[9], -1);
    EXPECT_EQ(buffer[15], -1);
}

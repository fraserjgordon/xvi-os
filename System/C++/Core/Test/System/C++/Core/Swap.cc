#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Swap.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Swap, SwapInts)
{
    int a = 1;
    int b = 2;

    swap(a, b);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 1);
}

TEST(Swap, ExchangeInts)
{
    int a = 1;
    int b = exchange(a, 2);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 1);
}

#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Swap.hh>

// Needed for the array implementation of ranges::swap.
#include <System/C++/Core/IteratorUtils.hh>


using namespace __XVI_STD_CORE_NS;


struct weird_swappable { int val; };

constexpr void swap(weird_swappable& a, weird_swappable& b)
{
    // Some non-swapping behaviour so we can make sure this overload was called.
    a.val += 5;
    b.val += 10;
}


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

TEST(Swap, RangesSwap)
{
    // Case 1: overloaded swap implementation.
    {
        weird_swappable a{1};
        weird_swappable b{2};

        ranges::swap(a, b);

        EXPECT_EQ(a.val, 6);
        EXPECT_EQ(b.val, 12);
    }

    // Case 2: arrays.
    {
        int a[3] = {1, 2, 3};
        int b[3] = {4, 5, 6};

        ranges::swap(a, b);

        EXPECT_EQ(a[0], 4);
        EXPECT_EQ(a[1], 5);
        EXPECT_EQ(a[2], 6);
        EXPECT_EQ(b[0], 1);
        EXPECT_EQ(b[1], 2);
        EXPECT_EQ(b[2], 3);
    }

    // Case 3: all other types.
    {
        int a = 1;
        int b = 2;

        ranges::swap(a, b);

        EXPECT_EQ(a, 2);
        EXPECT_EQ(b, 1);
    }
}

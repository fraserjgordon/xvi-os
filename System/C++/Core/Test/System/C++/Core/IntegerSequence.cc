#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/IntegerSequence.hh>


using namespace __XVI_STD_CORE_NS;


TEST(IntegerSequence, Size)
{
    EXPECT_EQ(index_sequence<>::size(), 0);
    EXPECT_EQ((index_sequence<0, 1, 2, 3, 4, 5>::size()), 6);
}

TEST(IntegerSequence, Make)
{
    using IS4 = index_sequence<0, 1, 2, 3>;

    EXPECT_SAME_TYPE(index_sequence<>, make_index_sequence<0>);
    EXPECT_SAME_TYPE(index_sequence<0>, make_index_sequence<1>);
    EXPECT_SAME_TYPE(IS4, make_index_sequence<4>);
}

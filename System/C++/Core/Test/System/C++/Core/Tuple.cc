#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Tuple.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Tuple, CommonReference)
{
    struct A {};
    struct B : A {};
    struct C {};

    using T = tuple<A&, B&, C&>;
    using U = tuple<B&, A&, C&>;
    using V = tuple<A&, A&, C&>;

    using Common = common_reference_t<T, U>;

    EXPECT_SAME_TYPE(Common, V);
}

TEST(Tuple, CommonType)
{
    using T = tuple<int, long, short>;
    using U = tuple<int, int, int>;
    using V = tuple<int, long, int>;

    using Common = common_type_t<T, U>;

    EXPECT_SAME_TYPE(Common, V);
}

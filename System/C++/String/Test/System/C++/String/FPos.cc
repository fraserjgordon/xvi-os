#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/FPos.hh>


using namespace __XVI_STD_STRING_NS;



TEST(Fpos, Types)
{
    using mbstate_t = __XVI_STD_STRING_NS::mbstate_t;

    EXPECT_SAME_TYPE(streampos, fpos<mbstate_t>);
    EXPECT_SAME_TYPE(wstreampos, fpos<mbstate_t>);
    EXPECT_SAME_TYPE(u8streampos, fpos<mbstate_t>);
    EXPECT_SAME_TYPE(u16streampos, fpos<mbstate_t>);
    EXPECT_SAME_TYPE(u32streampos, fpos<mbstate_t>);
}

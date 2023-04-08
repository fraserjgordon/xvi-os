#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/MBState.hh>


using namespace __XVI_STD_STRING_NS;


TEST(Mbstate, Size)
{
    EXPECT_EQ(sizeof(__XVI_STD_STRING_NS::mbstate_t), 8);
}

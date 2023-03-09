#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/AddressOf.hh>


using namespace __XVI_STD_CORE_NS;


TEST(AddressOf, OverloadedAddressOp)
{
    class funky { void operator&() = delete; };

    union u
    {
        funky f;
        int i;
    };

    u test;

    EXPECT_EQ(static_cast<void*>(addressof(test.f)), static_cast<void*>(&test.i));
}

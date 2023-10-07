#include <Tools/Development/Test/Test.hh>

#include <System/C++/Ranges/Algorithm.hh>


using namespace __XVI_STD_RANGES_NS::ranges;


TEST(Algorithm, AllOf)
{
    auto arr = {1, 2, 3, 4, 5};

    auto even = [](int x) { return ((x & 1) == 0); };
    auto small = [](int x) { return (x < 10); };

    auto twice = [](int x) { return (x * 2); };

    ASSERT(all_of(arr.begin(), arr.end(), small));
    ASSERT(!all_of(arr.begin(), arr.end(), even));

    ASSERT(all_of(arr, small));
    ASSERT(!all_of(arr, even));

    ASSERT(!all_of(arr, small, twice));
    ASSERT(all_of(arr, even, twice));
}

#include <Tools/Development/Test/Test.hh>

#include <System/C++/Algorithm/Algorithm.hh>


using namespace __XVI_STD_ALGORITHM_NS;


TEST(Algorithm, AllOf)
{
    auto arr = {1, 2, 3, 4, 5};
    auto even = [](int x) { return ((x & 1) == 0); };
    auto small = [](int x) { return (x < 10); };

    ASSERT(all_of(arr.begin(), arr.end(), small));
    ASSERT(!all_of(arr.begin(), arr.end(), even));
}

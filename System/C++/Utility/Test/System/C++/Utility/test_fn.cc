#include <System/C++/Utility/Function.hh>

using namespace std;

function<int(int,int)> test_fn = [](int a, int b) { return a + b; };

int invoke_test_fn()
{
    return test_fn(30, 12);
}


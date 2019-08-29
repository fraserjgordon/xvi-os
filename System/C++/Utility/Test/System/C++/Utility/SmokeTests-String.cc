#include <System/C++/Utility/String.hh>

using namespace __XVI_STD_NS;

extern "C" int printf(const char*, ...);

extern "C" void doStringTest()
{
    printf("\"%s\"\n", "Hello, World!"s.c_str());
    printf("\"%s\"\n", "This line is long and gets heap allocated"s.c_str());
    printf("\"%s\" + \"%s\" = \"%s\"\n", "Hello, ", "World!", ("Hello, "s + "World!"s).c_str());
    printf("\"%s\" + \"%s\" + \"%s\" = \"%s\"\n", "Hello", ", ", "World!", ("Hello"s + ", "s + "World!"s).c_str());
    
    printf("\"%s\" + \"%s\" + \"%s\" + \"%s\" + \"%s\" = \"%s\"\n",
        "Neko is the",
        " ossumest",
        " and bestest",
        " and moast perfect",
        " wife EVAR!",
        ("Neko is the"s + " ossumest" + " and bestest" + " and moast perfect" + " wife EVAR!").c_str());
}

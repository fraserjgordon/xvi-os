#include <Tools/Development/Test/Test.hh>


TESTLIBNAME("Tools.Development.Test")


int main(int argc, char** argv, char** envp)
{
    return Tools::Development::Test::runAllTests();
}

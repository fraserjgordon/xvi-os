#include <System/ABI/C++/Virtual.hh>

#include <System/ABI/C++/Exception.hh>


namespace __SYSTEM_ABI_CXX_NS
{


// Bind weakly to avoid a circular reference.
[[gnu::weak, noreturn]] void terminate() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.Terminate);

void __cxa_pure_virtual()
{
    terminate();
}


} // namespace __SYSTEM_ABI_CXX_NS

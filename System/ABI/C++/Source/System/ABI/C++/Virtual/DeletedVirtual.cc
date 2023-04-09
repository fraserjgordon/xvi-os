#include <System/ABI/C++/Virtual.hh>


namespace __SYSTEM_ABI_CXX_NS
{


// Bind weakly to avoid a circular reference.
[[gnu::weak, noreturn]] void terminate() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.Terminate);

void __cxa_deleted_virtual()
{
    terminate();
}


} // namespace __SYSTEM_ABI_CXX_NS

#ifndef __SYSTEM_ABI_TLS_TERMINATE_H
#define __SYSTEM_ABI_TLS_TERMINATE_H


namespace System::ABI::TLS
{

// System.ABI.C++ depends on this library but this library also depends on it. Weakly import the terminate()
// method from it (it'll always be available).
extern "C" [[gnu::weak, noreturn]] void __terminate() asm("System.ABI.CXX.Exception.Terminate");

} // namespace System::ABI::TLS


#endif /* ifndef __SYSTEM_ABI_TLS_TERMINATE_H */

#pragma once
#ifndef __SYSTEM_CXX_RUNTIME_SIGNAL_H
#define __SYSTEM_CXX_RUNTIME_SIGNAL_H


#include <System/C++/Runtime/Private/Config.hh>


namespace __XVI_STD_RUNTIME_NS
{


using sig_atomic_t = long;


namespace __detail
{

extern "C" using __signal_handler = void(int);

} // namespace __detail


__detail::__signal_handler* signal(int __signal, __detail::__signal_handler* __handler);
int raise(int __signal);


} // namespace __XVI_STD_RUNTIME_NS


#endif /* ifndef __SYSTEM_CXX_RUNTIME_SIGNAL_H */

#ifndef __SYSTEM_ABI_CXX_EXCEPTION_UTILS_H
#define __SYSTEM_ABI_CXX_EXCEPTION_UTILS_H


#include <System/ABI/C++/Exception.hh>


using namespace __SYSTEM_ABI_CXX_NS;
using namespace System::ABI::CXX;


#ifdef __SYSTEM_ABI_CXX_AEABI
using _Unwind_T = _Unwind_Control_Block;
using _Exception_T = _Unwind_Control_Block;
#else
using _Unwind_T = _Unwind_Exception;
using _Exception_T = void;
#endif


static inline std::uint64_t exceptionClass(_Unwind_T* e)
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    return e->__exception_class;
#else
    return e->exception_class;
#endif
}


// Gets a __cxa_exception header from an _Unwind_Exception header
inline __cxa_exception* getCxaException(_Unwind_T* e)
{
    // The _Unwind_Exception header is the last field in the __cxa_exception header.
    return reinterpret_cast<__cxa_exception*>(e + 1) - 1;
}

inline __cxa_exception* getCxaException(void* e)
{
    // Pointer points immediately after the __cxa_exception header.
    return reinterpret_cast<__cxa_exception*>(e) - 1;
}

inline _Unwind_T* unwindHeader(_Exception_T* e)
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    return &getCxaException(e)->ucb;
#else
    return &getCxaException(e)->unwindHeader;
#endif
}

inline bool isNative(_Unwind_T* e)
{
    auto eclass = exceptionClass(e);
    return (eclass == CxxExceptionClass || eclass == CxxDependentClass);
}

#ifndef __SYSTEM_ABI_CXX_AEABI
inline bool isNative(void* e)
{
    return isNative(unwindHeader(e));
}
#endif


#endif /* ifndef __SYSTEM_ABI_CXX_EXCEPTION_UTILS_H */

#include <System/ABI/C++/Unwinder.hh>

#include <System/ABI/C++/Unwind/Unwinder.hh>


using namespace System::ABI::CXX;


_Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Exception* e)
{
    return Unwinder::instance()->raise(e);
}

_Unwind_Reason_Code _Unwind_ForcedUnwind(_Unwind_Exception* e, _Unwind_Stop_Fn stop, void* stop_param)
{
    return Unwinder::instance()->forcedUnwind(e, stop, stop_param);
}

void _Unwind_Resume(_Unwind_Exception* e)
{
    Unwinder::Exception::from(e)->resume();
}

#ifndef __SYSTEM_ABI_CXX_AEABI
_Unwind_Reason_Code _Unwind_Resume_or_Rethrow(_Unwind_Exception* e)
{
    auto exception = Unwinder::Exception::from(e);
    if (exception->context()->isForced())
    {
        // This shouldn't return but we need to handle that possibility.
        exception->resume();
        return _URC_FATAL_PHASE2_ERROR;
    }

    // Otherwise re-raise the exception (i.e. treat it as a newly-thrown exception).
    return Unwinder::instance()->raise(e);
}
#endif

void _Unwind_DeleteException(_Unwind_Exception* e)
{
    if (e->exception_cleanup)
        e->exception_cleanup(_URC_FOREIGN_EXCEPTION_CAUGHT, e);
}

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetGR(_Unwind_Context* c, int reg)
{
    return Unwinder::Context::from(c)->getGR(reg);
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
void _Unwind_SetGR(_Unwind_Context* c, int reg, std::uintptr_t value)
{
    Unwinder::Context::from(c)->setGR(reg, value);
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetIP(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getIP();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
void _Unwind_SetIP(_Unwind_Context* c, std::uintptr_t value)
{
    Unwinder::Context::from(c)->setIP(value);
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetLanguageSpecificData(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getLSDA();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetRegionStart(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getRegionStart();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetDataRelBase(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getDataRelBase();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetTextRelBase(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getTextRelBase();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetCFA(_Unwind_Context* c)
{
    return Unwinder::Context::from(c)->getCFA();
}
#endif

#ifndef __SYSTEM_ABI_CXX_AEABI
std::uintptr_t _Unwind_GetIPInfo(_Unwind_Context* c, int* ip_before)
{
    return Unwinder::Context::from(c)->getIPInfo(ip_before);
}
#endif

_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn trace, void* trace_param)
{
    return Unwinder::instance()->backtrace(trace, trace_param);
}

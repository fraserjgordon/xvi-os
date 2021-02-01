#include <System/ABI/C++/Unwind/Unwinder.hh>


namespace System::ABI::CXX
{


class NullUnwinder final :
    public Unwinder
{
public:

    _Unwind_Reason_Code raise(_Unwind_Exception*) final
    {
        return _URC_FATAL_PHASE1_ERROR;
    }

    _Unwind_Reason_Code forcedUnwind(_Unwind_Exception*, _Unwind_Stop_Fn, void*) final
    {
        return _URC_FATAL_PHASE2_ERROR;
    }

    _Unwind_Reason_Code backtrace(_Unwind_Trace_Fn, void*) final
    {
        return _URC_FATAL_PHASE1_ERROR;
    }

    void* findEnclosingFunction(void*) final
    {
        return nullptr;
    }


    static NullUnwinder* nullInstance()
    {
        return &s_nullInstance;
    }

protected:

    void resume(Exception*) final
    {
    }

private:

    static NullUnwinder s_nullInstance;
};

NullUnwinder NullUnwinder::s_nullInstance = {};


#if __SYSTEM_ABI_CXX_UNWIND_NULL
Unwinder* Unwinder::instance()
{
    return NullUnwinder::nullInstance();
}
#endif


} // namespace System::ABI::CXX

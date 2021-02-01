#pragma once
#ifndef __SYSTEM_ABI_CXX_UNWIND_UNWINDER_H
#define __SYSTEM_ABI_CXX_UNWIND_UNWINDER_H


#include <System/ABI/C++/Unwinder.hh>

#include <System/C++/TypeTraits/TypeTraits.hh>
#include <System/C++/Utility/UniquePtr.hh>


// Empty base class.
struct _Unwind_Context {};


namespace System::ABI::CXX
{


class Unwinder
{
public:

    class Exception;
    class Context;

    virtual _Unwind_Reason_Code raise(_Unwind_Exception*) = 0;
    virtual _Unwind_Reason_Code forcedUnwind(_Unwind_Exception*, _Unwind_Stop_Fn, void*) = 0;
    virtual _Unwind_Reason_Code backtrace(_Unwind_Trace_Fn fn, void* trace_param) = 0;

    virtual void* findEnclosingFunction(void*) = 0;


    static Unwinder* instance();

protected:

    virtual ~Unwinder() = 0;

    virtual void resume(Exception*) = 0;

    // Custom operator delete to avoid dependency on the global delete.
    void operator delete(void*);
};


class Unwinder::Context :
    public _Unwind_Context
{
public:

    virtual ~Context() = 0;

    virtual Unwinder*     unwinder() = 0;

    virtual std::uintptr_t getGR(int reg) = 0;
    virtual void           setGR(int reg, std::uintptr_t value) = 0;

    virtual std::uintptr_t getIP() = 0;
    virtual void           setIP(std::uintptr_t value) = 0;

    virtual std::uintptr_t getLSDA() = 0;
    virtual std::uintptr_t getRegionStart() = 0;
    virtual std::uintptr_t getDataRelBase() = 0;
    virtual std::uintptr_t getTextRelBase() = 0;
    virtual std::uintptr_t getCFA() = 0;
    virtual std::uintptr_t getIPInfo(int*) = 0;

    virtual bool isForced() = 0;

    // Custom new/delete operators to avoid a circular dependency with the C++ standard library. They are also declared
    // as noexcept because throwing a bad_alloc exception would result in recursion.
    void* operator new(std::size_t) noexcept;
    void  operator delete(void*, std::size_t) noexcept;

    static Context* from(_Unwind_Context* c)
    {
        return reinterpret_cast<Context*>(c);
    }
};


class Unwinder::Exception
{
public:

    Context* context() const
    {
        return reinterpret_cast<Context*>(header()->private1);
    }

    std::unique_ptr<Context> takeContext()
    {
        std::unique_ptr<Context> ptr(context());
        header()->private1 = 0;
        return ptr;
    }

    template <class T>
        requires std::is_base_of_v<Context, T>
    void setContext(std::unique_ptr<T> context)
    {
        header()->private1 = reinterpret_cast<std::uintptr_t>(context.release());
    }

    std::uintptr_t handlerCFA() const
    {
        return header()->private2;
    }

    void setHandlerCFA(std::uintptr_t cfa)
    {
        header()->private2 = cfa;
    }

    const _Unwind_Exception* header() const
    {
        return &m_header;
    }

    _Unwind_Exception* header()
    {
        return &m_header;
    }

    void resume()
    {
        context()->unwinder()->resume(this);
    }


    static Exception* from(_Unwind_Exception* e)
    {
        return reinterpret_cast<Exception*>(e);
    }

protected:

    _Unwind_Exception m_header;

private:

    ~Exception() {}
};


} // namespace System::ABI::CXX


#endif /* ifndef __SYSTEM_ABI_CXX_UNWIND_UNWINDER_H */

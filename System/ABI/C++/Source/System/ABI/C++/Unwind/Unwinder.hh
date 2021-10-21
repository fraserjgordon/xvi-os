#pragma once
#ifndef __SYSTEM_ABI_CXX_UNWIND_UNWINDER_H
#define __SYSTEM_ABI_CXX_UNWIND_UNWINDER_H


#include <System/ABI/C++/Unwinder.hh>

#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>
#include <System/C++/Utility/UniquePtr.hh>


// Which unwinder to use by default?
#if !defined(__SYSTEM_ABI_CXX_AEABI)
#  if !defined(__SYSTEM_ABI_CXX_UNWIND_NULL)
#    define __SYSTEM_ABI_CXX_UNWIND_DWARF
#  endif
#endif


// Bridging between the SysV ABI and the ARM EHABI.
#ifdef __SYSTEM_ABI_CXX_AEABI
using _Unwind_Exception = _Unwind_Control_Block;
#endif


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
#ifdef __SYSTEM_ABI_CXX_AEABI
        return reinterpret_cast<Context*>(header()->unwinder_cache.reserved1);
#else
        return reinterpret_cast<Context*>(header()->private1);
#endif
    }

    std::unique_ptr<Context> takeContext()
    {
        std::unique_ptr<Context> ptr(context());
#ifdef __SYSTEM_ABI_CXX_AEABI
        header()->unwinder_cache.reserved1 = 0;
#else
        header()->private1 = 0;
#endif
        return ptr;
    }

    template <class T>
        requires std::is_base_of_v<Context, T>
    void setContext(std::unique_ptr<T> context)
    {
        auto value = reinterpret_cast<std::uintptr_t>(context.release());
#ifdef __SYSTEM_ABI_CXX_AEABI
        header()->unwinder_cache.reserved1 = value;
#else
        header()->private1 = value;
#endif
    }

    std::uintptr_t handlerCFA() const
    {
#ifdef __SYSTEM_ABI_CXX_AEABI
        return header()->unwinder_cache.reserved2;
#else
        return header()->private2;
#endif
    }

    void setHandlerCFA(std::uintptr_t cfa)
    {
#ifdef __SYSTEM_ABI_CXX_AEABI
        header()->unwinder_cache.reserved2 = cfa;
#else
        header()->private2 = cfa;
#endif
    }

#ifdef __SYSTEM_ABI_CXX_AEABI
    void setFrameIP(std::uintptr_t ip)
    {
        header()->unwinder_cache.reserved3 = ip;
    }

    std::uintptr_t getFrameIP() const
    {
        return header()->unwinder_cache.reserved3;
    }
#endif

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

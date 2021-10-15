#pragma once
#ifndef __SYSTEM_ABI_CXX_EXCEPTION_H
#define __SYSTEM_ABI_CXX_EXCEPTION_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/ABI/C++/Private/Config.hh>
#include <System/ABI/C++/TypeInfo.hh>
#include <System/ABI/C++/Unwinder.hh>


namespace __cxxabiv1
{

extern "C"
{


struct __cxa_exception
{
    std::atomic<std::size_t> referenceCount             = 0;

    const std::type_info*   exceptionType               = nullptr;
    void (*exceptionDestructor)(void*)                  = nullptr;
    std::unexpected_handler unexpectedHandler           = nullptr;
    std::terminate_handler  terminateHandler            = nullptr;
    __cxa_exception*        nextException               = nullptr;

    int                     handlerCount                = 0;

#if defined(__SYSTEM_ABI_CXX_AEABI)
    int                     propagationCount            = 0;
    __cxa_exception*        nextPropagatingException    = nullptr;

    _Unwind_Control_Block   ucb                         = {};
#else
    int                     handlerSwitchValue          = 0;
    const char*             actionRecord                = nullptr;
    const char*             languageSpecificDataArea    = nullptr;
    void*                   catchTemp                   = nullptr;
    void*                   adjustedPointer             = nullptr;
    
    _Unwind_Exception       unwindHeader                = {};
#endif
};

struct __cxa_dependent_exception
{
    void*                   primaryException            = nullptr;

    std::unexpected_handler unexpectedHandler           = nullptr;
    std::terminate_handler  terminateHandler            = nullptr;
    __cxa_exception*        nextException               = nullptr;

    int                     handlerCount                = 0;

#if defined(__SYSTEM_ABI_CXX_AEABI)
    int                     propagationCount            = 0;
    __cxa_exception*        nextPropagatingException    = nullptr;

    _Unwind_Control_Block   ucb                         = {};
#else
    int                     handlerSwitchValue          = 0;
    const char*             actionRecord                = nullptr;
    const char*             languageSpecificDataArea    = nullptr;
    void*                   catchTemp                   = nullptr;
    void*                   adjustedPointer             = nullptr;
    
    _Unwind_Exception       unwindHeader                = {};
#endif
};

struct __cxa_eh_globals
{
    __cxa_exception*        caughtExceptions            = nullptr;
    unsigned int            uncaughtExceptions          = 0;

#if defined(__SYSTEM_ABI_CXX_AEABI)
    __cxa_exception*        propagatingExceptions       = nullptr;
#endif
};

// The only permitted use of this class is in a catch clause; any expression involving this type (e.g. taking the
// address of a __foreign_exception) will result in undefined behaviour.
struct __SYSTEM_ABI_CXX_EXCEPTION_EXPORT __foreign_exception
{
private:

    // Present to make this a polymorphic class but should never be called.
    virtual ~__foreign_exception() = 0;
};

// Defined by the ARM EHABI rather than the generic ABI.
enum __cxa_type_match_result
{
    __ctm_failed                        = 0,
    __ctm_succeeded                     = 1,
    __ctm_succeeded_with_ptr_to_base    = 2,
};


__SYSTEM_ABI_CXX_EXCEPTION_EXPORT __cxa_eh_globals*     __cxa_get_globals();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT __cxa_eh_globals*     __cxa_get_globals_fast();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                 __cxa_allocate_exception(std::size_t);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                  __cxa_free_exception(void*);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void     __cxa_throw(void*, std::type_info*, void (*)(void*));
#if __SYSTEM_ABI_CXX_AEABI
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                 __cxa_begin_catch(_Unwind_Control_Block*);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                 __cxa_get_exception_ptr(_Unwind_Control_Block*);
#else
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                 __cxa_get_exception_ptr(void*);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                 __cxa_begin_catch(void*);
#endif
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                  __cxa_end_catch();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void     __cxa_rethrow();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void     __cxa_bad_cast();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void     __cxa_bad_typeid();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT const std::type_info* __cxa_current_exception_type();

// Functions used only by the ARM EHABI.
#if __SYSTEM_ABI_CXX_AEABI
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                  __cxa_end_cleanup();
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT bool                  __cxa_begin_cleanup(_Unwind_Control_Block*);
#endif

// Functions defined only in the ARM EHABI and not the generic EHABI but which might be generally useful.
#if __SYSTEM_ABI_CXX_AEABI
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT __cxa_type_match_result   __cxa_type_match(_Unwind_Control_Block*, const std::type_info*, bool, void**);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void         __cxa_call_terminate(_Unwind_Control_Block*);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void         __cxa_call_unexpected(_Unwind_Control_Block*);
#else
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT __cxa_type_match_result   __cxa_type_match(void*, const std::type_info*, bool, void**);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void         __cxa_call_terminate(void*);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void         __cxa_call_unexpected(void*);
#endif

// Not documented in the exception handling ABI.
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT __cxa_dependent_exception*  __cxa_allocate_dependent_exception() noexcept;
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                        __cxa_free_dependent_exception(__cxa_dependent_exception*) noexcept;


} // extern "C"


} // namespace __cxxabiv1


namespace System::ABI::CXX
{


constexpr std::uint64_t CxxExceptionClass       = 0x58564900'432B2B00;  // 'XVI\0C++\0'
constexpr std::uint64_t CxxDependentClass       = 0x58564800'432B2B01;  // 'XVI\0C++\1'


__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void       terminate() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.Terminate);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void       terminateWithHandler(std::terminate_handler) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.TerminateWithHandler);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT std::terminate_handler  getTerminateHandler() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.GetTerminateHandler);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT std::terminate_handler  setTerminateHandler(std::terminate_handler) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.SetTerminateHandler);

__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void       unexpectedException() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.UnexpectedException);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void       unexpectedExceptionWithHandler(std::unexpected_handler) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.UnexpectedExceptionWithHandler);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT std::unexpected_handler getUnexpectedExceptionHandler() __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.GetUnexpectedExceptionHandler);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT std::unexpected_handler setUnexpectedExceptionHandler(std::unexpected_handler) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.SetUnexpectedExceptionHandler);

__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                   getCurrentException() noexcept __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.GetCurrentException);

__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void*                   createException(std::size_t, const std::type_info*, void (*destructor)(void*)) noexcept __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.CreateException);

__SYSTEM_ABI_CXX_EXCEPTION_EXPORT [[noreturn]] void       rethrowException(void*) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.RethrowException);

__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                    addExceptionRef(void*) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.AddExceptionRef);
__SYSTEM_ABI_CXX_EXCEPTION_EXPORT void                    releaseExceptionRef(void*) __SYSTEM_ABI_CXX_SYMBOL(System.ABI.CXX.Exception.ReleaseExceptionRef);


} // namespace System::ABI::CXX


#endif /* ifndef __SYSTEM_ABI_CXX_EXCEPTION_H */

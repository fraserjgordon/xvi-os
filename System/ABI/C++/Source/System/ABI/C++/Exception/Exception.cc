#include <System/ABI/C++/Exception.hh>

#include <System/C++/LanguageSupport/TypeInfo.hh>

#include <System/ABI/C++/Exception/Utils.hh>


// Only specific functions in this file are compiled with exception support (namely, those that call the 
// _Unwind_RaiseException function because the stack needs to be unwound through them).
#pragma GCC optimize ("no-exceptions")


namespace __cxxabiv1
{


using namespace System::ABI::CXX;


// Weakly-imported symbols.
[[gnu::weak]] void* __malloc(std::size_t) asm("System.Allocator.C.Malloc");
[[gnu::weak]] void* __free(void*) asm("System.Allocator.C.Free");


// Thread-specific exception state.
thread_local __cxa_eh_globals thread_ehGlobals;


#ifdef __SYSTEM_ABI_CXX_AEABI
using _Exception_T = _Unwind_Control_Block;
#else
using _Exception_T = void;
#endif


static inline __cxa_exception* fromObject(void* ptr)
{
    return reinterpret_cast<__cxa_exception*>(ptr) - 1;
}

static inline void* toObject(__cxa_exception* e)
{
    return e + 1;
}

static inline auto* toExceptionT(void* e)
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    return &fromObject(e)->ucb;
#else
    return e;
#endif
}

#ifdef __SYSTEM_ABI_CXX_AEABI
// AEABI-specific overload as most of its API takes an UCB pointer rather than the exception object pointer.
static inline __cxa_exception* fromObject(_Unwind_Control_Block* ptr)
{
    return reinterpret_cast<__cxa_exception*>(ptr + 1) - 1;
}
#endif

static inline auto* unwindHeader(__cxa_exception* ptr)
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    return &ptr->ucb;
#else
    return &ptr->unwindHeader;
#endif
}

template <class CxaExceptionT>
static inline void setupException(CxaExceptionT* e, std::uint64_t eclass, void (*cleanup)(_Unwind_Reason_Code, _Unwind_T*))
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    e->ucb.__exception_class = eclass;
    e->ucb.exception_cleanup = cleanup;
#else
    e->unwindHeader.exception_class = eclass;
    e->unwindHeader.exception_cleanup = cleanup;
#endif
}

static inline __cxa_dependent_exception* dependentCast(__cxa_exception* e)
{
    return reinterpret_cast<__cxa_dependent_exception*>(e + 1) - 1;
}

static inline __cxa_exception* primaryException(__cxa_exception* e)
{
    if (exceptionClass(unwindHeader(e)) == CxxExceptionClass)
        return e;
    else
        return fromObject(dependentCast(e)->primaryException);
}


static inline void incrementRefcount(__cxa_exception* e)
{
    e->referenceCount++;
}

static inline void decrementRefcount(__cxa_exception* e)
{
    // Decrement the reference count and check whether any references remain.
    if (--e->referenceCount == 0)
    {
        // No references left; the object needs to be deleted. Run the destructor and free the memory.
        if (e->exceptionDestructor)
            e->exceptionDestructor(e);
        __cxa_free_exception(toObject(e));
    }
}

static void exceptionCleanup(_Unwind_Reason_Code reason, _Unwind_T* exception)
{
    // This should only ever be called from a foreign runtime.
    if (reason != _URC_FOREIGN_EXCEPTION_CAUGHT)
        terminate();

    // Is this a dependent exception?
    if (exceptionClass(exception) == CxxExceptionClass)
    {
        // Decrease the parent's reference count and free the exception.
        auto header = fromObject(exception + 1);
        auto dependent = dependentCast(header);
        decrementRefcount(primaryException(header));
        __cxa_free_dependent_exception(dependent);
        return;
    }

    // Get the pointer to the C++ exception header.
    auto header = fromObject(exception + 1);

    // Decrement the refcount on the exception (it may be referenced elsewhere).
    decrementRefcount(header);
}

[[noreturn]]
static void raiseExceptionFailed(_Unwind_Reason_Code, _Exception_T* exception)
{
    // All exception handling needs to call __cxa_begin_catch and terminating after a failed throw is no different.
    __cxa_begin_catch(exception);

    // Terminate.
    auto header = fromObject(exception);
    if (isNative(unwindHeader(header)))
        terminateWithHandler(header->terminateHandler);
    else
        terminate();
}


__cxa_eh_globals* __cxa_get_globals()
{
    // No one-time init needed by this library.
    return __cxa_get_globals_fast();
}

__cxa_eh_globals* __cxa_get_globals_fast()
{
    return &thread_ehGlobals;
}

void* __cxa_allocate_exception(std::size_t size)
{
    // Adjust the size to include the C++ exception header.
    size += sizeof(__cxa_exception);

    // Attempt an allocation from the heap.
    __cxa_exception* ptr = nullptr;
    if (__malloc)
        ptr = static_cast<__cxa_exception*>(__malloc(size));

    //! @TODO: fall back to an emergency pool.

    // Fail if no memory was allocated.
    if (!ptr)
        terminate();

    // Adjust the pointer to point to the user area.
    return toObject(ptr);
}

void __cxa_free_exception(void* object)
{
    // Get the real pointer.
    auto ptr = fromObject(object);

    //! @TODO: try to free via the emergency pool.

    // Return the memory to the heap.
    __free(ptr);
}

__cxa_dependent_exception* __cxa_allocate_dependent_exception() noexcept
{
    // Attempt to allocate from the heap.
    __cxa_dependent_exception* ptr = nullptr;
    if (__malloc)
        ptr = static_cast<__cxa_dependent_exception*>(__malloc(sizeof(__cxa_dependent_exception)));

    if (!ptr)
        terminate();

    return ptr;
}

void __cxa_free_dependent_exception(__cxa_dependent_exception* ptr) noexcept
{
    // Return the memory to the heap.
    __free(ptr);
}

const std::type_info* __cxa_current_exception_type()
{
    // Get the pointer to the C++ exception header.
    auto header = __cxa_get_globals()->caughtExceptions;
    if (!header)
        return nullptr;

    // What type of exception are we dealing with?
    switch (exceptionClass(unwindHeader(header)))
    {
        case CxxExceptionClass:
            // Get the type info from the header.
            return header->exceptionType;

        case CxxDependentClass:
        {
            // We need to look up the primary exception and get the type info from that.
            auto dependent = dependentCast(header);
            auto primary = fromObject(dependent->primaryException);
            return primary->exceptionType;
        }

        default:
            // Foreign exception - does not have any C++ type info.
            return nullptr;
    }
}

[[gnu::optimize("unwind-tables")]]
void __cxa_throw(void* exception, std::type_info* typeinfo, void (*destructor)(void*))
{
    // Get the pointer to the C++ exception header.
    auto header = fromObject(exception);

    // Fill in the C++ exception information.
    header->exceptionType = typeinfo;
    header->exceptionDestructor = destructor;
    header->unexpectedHandler = getUnexpectedExceptionHandler();
    header->terminateHandler = getTerminateHandler();

    // Take a reference to this exception.
    incrementRefcount(header);

    // Fill in the language-independent exception information.
    setupException(header, CxxExceptionClass, &exceptionCleanup);

#ifdef __SYSTEM_ABI_CXX_AEABI
    // The reserved1 field of the unwinder cache is required by the ABI to be set to zero at throw time.
    header->ucb.unwinder_cache.reserved1 = 0;
#endif

    // There is now another uncaught exception in flight.
    __cxa_get_globals_fast()->uncaughtExceptions++;

    // Start the unwinding process. This doesn't normally return.
    auto result = _Unwind_RaiseException(unwindHeader(header));

    raiseExceptionFailed(result, toExceptionT(exception));
}

void* __cxa_get_exception_ptr(_Exception_T* exception)
{
#ifdef __SYSTEM_ABI_CXX_AEABI
    // Return the adjusted pointer that was stored in the header.
    return reinterpret_cast<void*>(exception->barrier_cache.bitpattern[0]);
#else
    // Get the pointer to the C++ exception header.
    auto header = fromObject(exception);
    
    // Return the adjusted pointer that was stored in the header.
    return header->adjustedPointer;
#endif
}

void* __cxa_begin_catch(_Exception_T* exception)
{
    // Get the pointer to the C++ exception header.
    auto header = fromObject(exception);

    // Is this a native or foreign exception?
    if (isNative(unwindHeader(header)))
    {
        // Increment the catch count of this exception. It may have been negated in __cxa_rethrow and that needs to be
        // correctced here.
        if (header->handlerCount >= 0)
            header->handlerCount++;
        else
            header->handlerCount = -header->handlerCount + 1;

        // Add to the list of caught exceptions (if not already there).
        auto eh = __cxa_get_globals();
        if (eh->caughtExceptions != header)
        {
            header->nextException = eh->caughtExceptions;
            eh->caughtExceptions = header;
        }

        // There is one fewer uncaught exception in flight.
        eh->uncaughtExceptions--;

        // Return the adjusted object pointer.
#ifdef __SYSTEM_ABI_CXX_AEABI
        return __cxa_get_exception_ptr(unwindHeader(header));
#else
        return __cxa_get_exception_ptr(exception);
#endif
    }
    else
    {
        // We can only catch foreign exceptions when the catch stack is empty as we don't have a __cxa_exception header
        // to link into the list.
        auto eh = __cxa_get_globals();
        if (eh->caughtExceptions != nullptr)
            terminate();

        // Push the (incorrect) __cxa_exception pointer onto the stack (the ABI requires this, unfortunately).
        eh->caughtExceptions = header;

        // No change to the uncaught exception count - the exception is foreign so there won't have been a call to
        // __cxa_throw therefore the uncaught count won't have been incremented.

        // Return a pointer to the foreign exception object.
        return exception;
    }
}

void __cxa_end_catch()
{
    // Get the most recently caught exception from the catch stack.
    auto eh = __cxa_get_globals_fast();
    auto header = eh->caughtExceptions;

    // Do nothing if there are no entries in the caught list. This may be the case if a foreign exception was rethrown.
    if (!header)
        return;

    // Is this a native exception?
    if (!isNative(unwindHeader(header)))
    {
        // Pop the exception from the catch list. It must be the only entry on the list as it doesn't have a C++
        // exception header (__cxa_exception) to store the next exception in.
        eh->caughtExceptions = nullptr;

        // Delete the exception.
        _Unwind_DeleteException(unwindHeader(header));
    }

    // Is this a rethrow? (if so, the handler count will be negative).
    bool rethrow = (header->handlerCount < 0);
    if (rethrow)
    {
        // Rethrown exceptions have a negative handler count. Increment it and remove from the head of the caught list
        // if there are no longer any active handlers.
        bool done = (++header->handlerCount == 0);
        if (done)
            eh->caughtExceptions = header->nextException;

        // Because the exception is going to be rethrown, we don't decrement the refcount on the exception.
    }
    else
    {
        // Not a rethrow. Decrement the handler count.
        bool done = (--header->handlerCount == 0);
        if (done)
        {
            // Pop the exception from the caught stack.
            eh->caughtExceptions = header->nextException;

            // Is this a dependent exception?
            bool is_dependent = (exceptionClass(unwindHeader(header)) == CxxDependentClass);
            auto dependent = is_dependent ? dependentCast(header) : nullptr;
            auto primary = is_dependent ? fromObject(dependent->primaryException) : header;

            // Delete the dependent exception (if any).
            if (dependent)
                __cxa_free_dependent_exception(dependent);

            // Decrement the reference count of the primary exception (this may free it).
            decrementRefcount(primary);
        }
    }
}

#ifdef __SYSTEM_ABI_CXX_AEABI
bool __cxa_begin_cleanup(_Unwind_Control_Block* exception)
{
    // Add to the list of propagating exceptions.
    auto eh = __cxa_get_globals_fast();
    auto header = fromObject(exception);
    if (eh->propagatingExceptions != header)
    {
        header->nextPropagatingException = eh->propagatingExceptions;
        eh->propagatingExceptions = header;
    }

    // Increment the refcount.
    ++header->propagationCount;

    return true;
}

// Assembly wrapper for __cxa_end_cleanup.
asm
(R"(

.global __cxa_end_cleanup
.protected __cxa_end_cleanup
.type __cxa_end_cleanup, "function"
__cxa_end_cleanup:

    # Ensure that all potentially-significant registers are preserved across the call.
    push    {r0-r12, lr}

    # Call the real __cxa_end_cleanup function.
    bl      __cxa_end_cleanup_impl

    # Restore the registers.
    pop     {r0-r12, lr}

    # Tail-call into the unwinder with the correct register state. It shouldn't return.
    b       _Unwind_Resume

)");


[[gnu::used]] static void __cxa_end_cleanup_impl()
{
    // Remove this exception from the list of propagating exceptions in cleanup.
    auto eh = __cxa_get_globals_fast();
    auto header = eh->propagatingExceptions;
    if (!header)
        __cxa_call_terminate(nullptr);

    if (--header->propagationCount == 0)
    {
        eh->propagatingExceptions = header->nextPropagatingException;
        header->nextPropagatingException = nullptr;
    }

    // The assembly wrapper for this calls _Unwind_Resume after restoring the register state.
}
#endif

[[gnu::optimize("exceptions")]]
void __cxa_rethrow()
{
    // Get the most recently caught exception.
    auto eh = __cxa_get_globals_fast();
    auto header = eh->caughtExceptions;

    // Rethrowing with no active exception should result in a call to std::terminate.
    if (!header)
        terminate();

    // Is this a foreign exception?
    bool native = isNative(unwindHeader(header));
    if (native)
    {
        // Use a negative handler count to signal that we've requested a rethrow.
        header->handlerCount = -header->handlerCount;

        // This exception is now uncaught again.
        eh->uncaughtExceptions++;
    }
    else
    {
        // We don't have a real __cxa_exception header for this exception so we need some other way of communicating
        // that it is to be rethrown. We already have the restriction that a foreign exception cannot be thrown while
        // other exceptions are in flight so the catch stack contains exactly one exception.
        //
        // Combine this with the observation that calling __cxa_end_catch is not permitted outside of an active catch;
        // that allows us to use an empty catch stack as a signal that we're rethrowing a foreign exception.
        eh->caughtExceptions = nullptr;
    }

    // We're re-throwing so the catch has finished.
    __cxa_end_catch();

    // Inform the unwinder library that a "new" exception has been raised. (Although the unwinder library has seen this
    // exception before, it considered it to have been handled when it was caught so, from the unwinder's point of view,
    // this is a different exception).
    //
    // This method will only return if some fatal unwinding error occurred.
    auto result = _Unwind_RaiseException(unwindHeader(header));

    raiseExceptionFailed(result, toExceptionT(toObject(header)));
}

[[gnu::optimize("exceptions")]]
void __cxa_bad_cast()
{
    throw std::bad_cast();
}

[[gnu::optimize("exceptions")]]
void __cxa_bad_typeid()
{
    throw std::bad_typeid();
}


} // namespace __cxxabiv1


namespace System::ABI::CXX
{


void* getCurrentException() noexcept
{
    // Get the current exception, if one exists.
    auto header = __cxxabiv1::__cxa_get_globals()->caughtExceptions;
    if (!header)
        return nullptr;

    // Fail if the current exception is foreign.
    if (!isNative(unwindHeader(header)))
        return nullptr;

    // Always return a pointer to the primary exception, never a dependent one.
    return __cxxabiv1::primaryException(header) + 1;
}


void* createException(std::size_t size, const std::type_info* type, void (*destructor)(void*)) noexcept
{
    // Attempt to allocate space for the exception.
    auto* exception = __cxxabiv1::__cxa_allocate_exception(size);
    if (!exception)
        return nullptr;

    // Exception was allocated - fill in the C++ exception header.
    auto* header = __cxxabiv1::fromObject(exception);
    header->exceptionType = type;
    header->exceptionDestructor = destructor;
    header->unexpectedHandler = getUnexpectedExceptionHandler();
    header->terminateHandler = getTerminateHandler();

    // Add a single reference to the exception.
    __cxxabiv1::incrementRefcount(header);

    // Fill in the unwind header for a primary exception.
    setupException(header, __cxxabiv1::CxxExceptionClass, &__cxxabiv1::exceptionCleanup);

    return exception;
}


[[gnu::optimize("exceptions")]]
void rethrowException(void* e)
{
    // Terminate if the given exception is invalid.
    if (!e)
        terminate();    

    // We cannot rethrow foreign exceptions as a dependent exception.
    auto header = __cxxabiv1::fromObject(e);
    if (!isNative(unwindHeader(header)))
        terminate();

    // Ensure we're pointing to a primary exception.
    header = __cxxabiv1::primaryException(header);

    // Attempt to allocate a dependent exception object.
    //
    //! @TODO: what is the standard-mandated behaviour if rethrowing fails to allocate?
    auto dependent = __cxxabiv1::__cxa_allocate_dependent_exception();
    if (!dependent)
        terminate();

    // Fill in the C++ exception header of the dependent exception.
    dependent->primaryException = e;
    dependent->unexpectedHandler = getUnexpectedExceptionHandler();
    dependent->terminateHandler = getTerminateHandler();

    // Take a reference to the primary exception.
    incrementRefcount(header);

    // Fill in the unwind header.
    setupException(dependent, __cxxabiv1::CxxDependentClass, &__cxxabiv1::exceptionCleanup);

    // There is now another uncaught exception in flight.
    __cxxabiv1::__cxa_get_globals_fast()->uncaughtExceptions++;

    // Start the unwinding process. This doesn't normally return.
    auto result = _Unwind_RaiseException(unwindHeader(header));

    __cxxabiv1::raiseExceptionFailed(result, toExceptionT(dependent + 1));
}


void addExceptionRef(void* e)
{
    auto header = __cxxabiv1::fromObject(e);
    if (!isNative(unwindHeader(header)))
        terminate();

    __cxxabiv1::incrementRefcount(header);
}

void releaseExceptionRef(void* e)
{
    auto header = __cxxabiv1::fromObject(e);
    if (!isNative(unwindHeader(header)))
        terminate();

    __cxxabiv1::decrementRefcount(header);
}


} // namespace System::ABI::CXX

#include <System/C++/LanguageSupport/Exception.hh>

#include <System/ABI/C++/Exception.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


exception::~exception() = default;

const char* exception::what() const noexcept
{
    return "unknown exception";
}

const char* bad_exception::what() const noexcept
{
    return "bad exception";
}

const char* bad_alloc::what() const noexcept
{
    return "allocation failure";
}

const char* bad_array_new_length::what() const noexcept
{
    return "invalid length for array new";
}


terminate_handler set_terminate(terminate_handler new_handler) noexcept
{
    return System::ABI::CXX::setTerminateHandler(new_handler);
}

terminate_handler get_terminate() noexcept
{
    return System::ABI::CXX::getTerminateHandler();
}

unexpected_handler set_unexpected(unexpected_handler new_handler) noexcept
{
    return System::ABI::CXX::setUnexpectedExceptionHandler(new_handler);
}

unexpected_handler get_unexpected() noexcept
{
    return System::ABI::CXX::getUnexpectedExceptionHandler();
}


int uncaught_exceptions() noexcept
{
    const auto* globals = __cxxabiv1::__cxa_get_globals();
    auto uncaught = globals->uncaughtExceptions;

    // Hopefully we never have more than numeric_limits<int>::max() uncaught exceptions...
    return static_cast<int>(uncaught);
}


exception_ptr exception_ptr::__make(void* obj, size_t obj_size, const std::type_info* type, void (*destructor)(void*), void (*copy)(void*, const void*))
{
    // Attempt to allocate space for a copy of the object to be thrown.
    auto exception = System::ABI::CXX::createException(obj_size, type, destructor);
    if (!exception)
    {
        // Failed to allocate space for the exception. Try to allocate a bad_alloc exception instead.
        exception = System::ABI::CXX::createException(sizeof(bad_alloc), &typeid(bad_alloc), [](void* e)
        {
            static_cast<bad_alloc*>(e)->~bad_alloc();
        });

        if (!exception)
        {
            // Could not allocate the fall-back bad_alloc exception; die.
            System::ABI::CXX::terminate();
        }

        // Construct the bad_alloc instance and return a pointer to it.
        new (exception) bad_alloc();
        return __wrap(exception);
    }

    // Copy the object into place, catching any exceptions that it might throw.
    try
    {
        copy(exception, obj);
    }
    catch (...)
    {
        // The copy operation threw. We should return a pointer to that exception instead.
        __cxxabiv1::__cxa_free_exception(exception);
        return current_exception();
    }

    // A new exception_ptr object has been created successfully.
    return __wrap(exception);
}

exception_ptr exception_ptr::__wrap(void* obj)
{
    // Create an exception pointer pointing to the existing exception.
    exception_ptr __e = {};
    __e._M_ptr = obj;

    // No reference bump - we consume the reference that the caller held.

    return __e;
}


void exception_ptr::__ref() const noexcept
{
    System::ABI::CXX::addExceptionRef(_M_ptr);
}

void exception_ptr::__unref() const noexcept
{
    if (_M_ptr)
        System::ABI::CXX::releaseExceptionRef(_M_ptr);
}


exception_ptr current_exception() noexcept
{
    // Return an empty exception pointer if there is no current exception.
    auto current = System::ABI::CXX::getCurrentException();
    if (!current)
        return {};

    // Take a reference to the exception and wrap it into an exception pointer.
    auto ptr = exception_ptr::__wrap(current);
    ptr.__ref();
    return ptr;
}

void rethrow_exception(exception_ptr ep)
{
    System::ABI::CXX::rethrowException(ep.__get());
}


} // namespace 
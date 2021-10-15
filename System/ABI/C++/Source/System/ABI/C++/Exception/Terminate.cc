#include <System/ABI/C++/Exception.hh>

#include <System/C++/LanguageSupport/Exception.hh>

#include <System/ABI/C++/Exception/Utils.hh>


namespace System::ABI::CXX
{


// Weakly imported symbols.
[[noreturn]] [[gnu::weak]] void __abort() asm("System.Abort");


// Global terminate() handler.
static std::atomic<std::terminate_handler> g_terminateHandler  = nullptr;


void terminate()
{
    terminateWithHandler(g_terminateHandler);
}

void terminateWithHandler(std::terminate_handler handler)
{
    if (handler)
        handler();

    __abort();
}

std::terminate_handler getTerminateHandler()
{
    return g_terminateHandler;
}

std::terminate_handler setTerminateHandler(std::terminate_handler handler)
{
    return g_terminateHandler.exchange(handler);
}


}


namespace std
{

// Weak definition of std::terminate() in case the stdlib doesn't provide it.
[[gnu::weak]] void terminate()
{
    System::ABI::CXX::terminate();
}

} // namespace std


namespace __cxxabiv1
{


void __cxa_call_terminate(_Exception_T* exception)
{
    // Catch the exception then terminate, using the appropriate terminate handler.
    __cxa_begin_catch(exception);
    bool native_exception = isNative(exception);
    if (native_exception)
    {
        auto* cxa = getCxaException(exception);
        System::ABI::CXX::terminateWithHandler(cxa->terminateHandler);
    }
    else
    {
        System::ABI::CXX::terminate();
    }
}


} // namespace __cxxabiv1

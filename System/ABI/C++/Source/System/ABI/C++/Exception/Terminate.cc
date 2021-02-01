#include <System/ABI/C++/Exception.hh>

#include <System/C++/LanguageSupport/Exception.hh>


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

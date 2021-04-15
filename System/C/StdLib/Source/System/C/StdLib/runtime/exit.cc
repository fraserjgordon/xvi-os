#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/stddef.h>
#include <System/C++/Containers/Vector.hh>
#include <System/C++/Utility/Array.hh>


#ifndef __XVI_KERNEL


[[noreturn]] void __exit(int) asm("System.ABI.Runtime.Exit");


namespace System::C::StdLib
{


class AtExitHandlers
{
public:

    using handler_t = void (*)();

    // The C standard requires a minimum of 32 at_exit and at_quick_exit handlers (each) be supported so we statically
    // allocate enough space for those. After that, dynamic allocation is used.
    static constexpr size_t StaticHandlers = 32;

    // Adds a new handler.
    bool push(handler_t handler)
    {
        if (m_handlerCount < StaticHandlers)
        {
            m_staticHandlers[m_handlerCount++] = handler;
            return true;
        }
        else
        {
            try
            {
                //m_dynamicHandlers.push_back(handler);
                //++m_handlerCount;
                //return true;
            }
            catch (...)
            {
                return false;
            }

            return false;
        }
    }

    // Runs all of the handlers currently registered.
    void run()
    {
        while (m_handlerCount > 0)
        {
            // Get the next handler to run.
            handler_t handler;
            //if (m_handlerCount < StaticHandlers)
                handler = m_staticHandlers[m_handlerCount];
            //else
            //    handler = m_dynamicHandlers[m_handlerCount - StaticHandlers];

            // Decrease the handler count before running the handler. By doing this, we'll correctly execute any new
            // handlers that get pushed during the execution of this handler.
            --m_handlerCount;

            // Run the handler.
            (*handler)();
        }
    }

private:

    //! @todo add a mutex to protect the handlers.

    // The number of handlers that have been added thusfar.
    size_t                                  m_handlerCount = 0;

    // The statically-allocated handlers.
    std::array<handler_t, StaticHandlers>   m_staticHandlers;

    // The dynamically-allocated handlers.
    //std::vector<handler_t>                  m_dynamicHandlers;
};


static AtExitHandlers s_atexit;
static AtExitHandlers s_atquickexit;


} // namespace System::C::StdLib


static void __flush_stdio();

static void __do_atexit()
{
    System::C::StdLib::s_atexit.run();
}

static void __do_atquickexit()
{
    System::C::StdLib::s_atquickexit.run();
}


int atexit(void (*handler)())
{
    if (System::C::StdLib::s_atexit.push(handler))
        return 0;
    else
        return -1;
}

int at_quick_exit(void (*handler)())
{
    if (System::C::StdLib::s_atquickexit.push(handler))
        return 0;
    else
        return -1;
}

void exit(int status)
{
    __do_atexit();
    __flush_stdio();
    __exit(status);
}

void _Exit(int status)
{
    // Implementation-defined: do stdio buffers get flushed, files closed, temp files deleted?
    //
    // Answer: no.
    __exit(status);
}

void quick_exit(int status)
{
    __do_atquickexit();
    _Exit(status);
}


#endif // ifndef __XVI_KERNEL

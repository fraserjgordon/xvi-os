#include <System/ABI/C++/Unwinder.hh>
#include <System/ABI/Dwarf/CFI.hh>
#include <System/ABI/Dwarf/FDE.hh>
#include <System/C++/Containers/Deque.hh>
#include <System/C++/Containers/ForwardList.hh>
#include <System/C++/Containers/List.hh>
#include <System/C++/Containers/Map.hh>
#include <System/C++/Containers/Queue.hh>
#include <System/C++/Containers/Set.hh>
#include <System/C++/Containers/Stack.hh>
#include <System/C++/Format/Format.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/StdExcept.hh>

#include <System/Utility/Logger/Logger.hh>
#include <System/Utility/Logger/Processor.hh>

using namespace System::Utility::Logger;


/*template class std::map<int, std::string>;
template class std::multimap<int, std::string>;
template class std::deque<std::string>;
template class std::set<std::string>;
template class std::multiset<std::string>;
template class std::vector<std::string>;
template class std::list<std::string>;
template class std::forward_list<std::string>;
template class std::queue<std::string>;
template class std::stack<std::string>;*/


extern "C"
{

void* malloc(std::size_t);
void free(void*);
int printf(const char*, ...);


[[gnu::visibility("hidden")]] extern std::byte _ehframehdr asm("__GNU_EH_FRAME_HDR");
[[gnu::visibility("hidden")]] extern std::byte _text asm (".text");
[[gnu::visibility("hidden")]] extern std::byte _data asm (".data");
[[gnu::visibility("hidden")]] extern std::byte _ehframe asm (".eh_frame");

static constinit auto _ehframehdr_ptr = &_ehframehdr;


void* _malloc(std::size_t size) asm("System.Allocator.C.Malloc");
void _free(void* ptr) asm ("System.Allocator.C.Free");


void* _malloc(std::size_t size)
{
    return malloc(size);
}

void _free(void* ptr)
{
    return free(ptr);
}

void test()
{
    try
    {
        throw 42;
    }
    catch (std::bad_alloc&)
    {
    }
}

void doLog(void*, opaque_message msg)
{
    auto [str, len] = getMessageString(msg);
    printf("%.*s", len, str);
}

namespace Dwarf = System::ABI::Dwarf;

[[gnu::visibility("protected")]] int dotest() noexcept
{
    /*auto t = std::uintptr_t(&_text);
    auto d = std::uintptr_t(&_data);

    printf("Staring unwind test...\n");

    printf("Registering .eh_frame_hdr section...");
    _Unwind_XVI_RegisterEhFrameHdr(_ehframehdr_ptr, t, d);
    printf(" done\n");

    printf("Testing backtrace unwind...\n");
    auto trace = [&](_Unwind_Context* context)
    {
        auto pc = _Unwind_GetIP(context);
        auto cfa = _Unwind_GetCFA(context);
        printf("    --> PC=%p, CFA=%p\n", pc, cfa);
        return _URC_CONTINUE_UNWIND;
    };
    auto trace_wrapper = [](_Unwind_Context* context, void* param)
    {
        auto fn = *reinterpret_cast<decltype(trace)*>(param);
        return fn(context);
    };
    auto bt_result = _Unwind_Backtrace(trace_wrapper, &trace);
    printf("    --> end of stack (%u)\n", bt_result);

    printf("\n");
    printf("================================================================================\n");
    printf("======================= Dumping all CIE and FDE records ========================\n");
    printf("================================================================================\n");
    printf("\n");

    const auto* ptr = &_ehframe;
    Dwarf::DwarfCIE cie;
    while (true)
    {
        auto e = Dwarf::DwarfCieOrFde::DecodeFrom(ptr, t, d);
        if (!e.isValid())
            break;
        if (e.is_fde)
        {
            printf("%s", e.fde.toString().c_str());
            printf("%s", Dwarf::DisassembleCFI(cie, e.fde.getInstructions(), e.fde.getInstructionsLength()).c_str());
            printf("%s", Dwarf::PrintRegisterRulesTable(e.fde).c_str());
        }
        else
        {
            cie = e.cie;
            printf("%s", e.cie.toString().c_str());
            printf("%s", Dwarf::DisassembleCFI(cie, cie.getInstructions(), cie.getInstructionsLength()).c_str());
        }
    }*/

    /*printf("\n");

    try { test(); } 
    catch (float) {}
    catch (const std::string&) {}
    catch (int i)
    {
        printf("%d\n", i);
    }
    catch (...)
    {
        printf("wrong handler :-(\n");
    }


    printf("Unwind test done\n");*/

    registerMessageProcessor(&doLog, nullptr);

    log(DefaultFacility, priority::info, "Test log message. Meaning is {}", 
        {{"AppName","test"}, {"Works","yes!"}},
        42
    );


    return 0;
}

}

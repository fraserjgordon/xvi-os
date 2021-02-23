#include <System/ABI/C++/Unwind/Unwinder.hh>

#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::CXX
{


struct index_entry
{
    // Special data value to indicate that a frame cannot be unwound.
    static constexpr std::uintptr_t CannotUnwind    = 0x1;

    std::uintptr_t  offset;
    std::uintptr_t  data;
};


class EHABIUnwinder final :
    public Unwinder
{

};


} // namespace System::ABI::CXX

#include <System/C++/LanguageSupport/Exception.hh>

#include <System/ABI/C++/Exception.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


void terminate()
{
    System::ABI::CXX::terminate();
}


} // namespace __XVI_STD_LANGSUPPORT_NS

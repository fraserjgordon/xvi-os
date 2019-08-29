#include <System/C++/LanguageSupport/CLimits.hh>
#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/Contract.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdArg.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>
#include <System/C++/LanguageSupport/Utility.hh>

using namespace __XVI_STD_NS;

static_assert(is_same_v<common_comparison_category_t<>, strong_ordering>);
static_assert(is_same_v<common_comparison_category_t<weak_equality, strong_equality>, weak_equality>);

void test_throw_with_nested()
{
    class foo {};
    throw_with_nested(foo{});
}

#include <System/C++/Utility/StdLib.hh>

#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/StdLib/voidptr_iter.hh>


namespace __XVI_STD_UTILITY_NS
{


void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, __detail::__c_compare_pred* pred)
{
    auto wrapped_pred = [pred](const byte& a, const byte& b) -> bool
    {
        return pred(&a, &b) < 0;
    };

    voidptr_iter first = {base, static_cast<voidptr_iter::difference_type>(size)};
    voidptr_iter last = first + static_cast<voidptr_iter::difference_type>(nmemb);

    auto value = *reinterpret_cast<const byte*>(key);

    auto result = lower_bound(first, last, value, wrapped_pred);

    if (result == last || wrapped_pred(value, *first))
        return nullptr;

    return &*first;
}

} // namespace __XVI_STD_UTILITY_NS

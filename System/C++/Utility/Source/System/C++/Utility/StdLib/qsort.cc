#include <System/C++/Utility/StdLib.hh>

#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/StdLib/voidptr_iter.hh>


namespace __XVI_STD_UTILITY_NS
{


void qsort(void* base, size_t nmemb, size_t size, __detail::__c_compare_pred* pred)
{
    auto wrapped_pred = [pred](const byte& a, const byte& b) -> bool
    {
        return pred(&a, &b) < 0;
    };

    voidptr_iter first = {base, static_cast<voidptr_iter::difference_type>(size)};
    voidptr_iter last = first + static_cast<voidptr_iter::difference_type>(nmemb);

    sort(first, last, wrapped_pred);
}



} // namespace __XVI_STD_UTILITY_NS

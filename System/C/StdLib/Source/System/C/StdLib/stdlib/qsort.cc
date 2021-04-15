#include <System/C/StdLib/stdlib.h>

#include <System/C++/Utility/Algorithm.hh>

#include <System/C/StdLib/stdlib/voidptr_iter.hh>


void qsort(void* base, size_t nmemb, size_t size, int (*pred)(const void*, const void*))
{
    using std::byte;
    using System::C::StdLib::voidptr_iter;    

    auto wrapped_pred = [pred](const byte& a, const byte& b) -> bool
    {
        return pred(&a, &b) < 0;
    };

    voidptr_iter first = {base, static_cast<voidptr_iter::difference_type>(size)};
    voidptr_iter last = first + static_cast<voidptr_iter::difference_type>(nmemb);

    std::sort(first, last, wrapped_pred);
}

#include <System/C/StdLib/string.h>

#include <System/ABI/String/String.hh>

#include <System/C/StdLib/stdlib.h>


char* strndup(const char* str, size_t n)
{
    if (!str || n == 0)
        return nullptr;

    auto len = __strnlen(str, n);
    if (len == 0)
        return nullptr;

    auto ptr = reinterpret_cast<char*>(malloc(len + 1));
    if (!ptr)
        return nullptr;

    __memcpy(ptr, str, len);
    ptr[len] = '\0';

    return ptr;
}

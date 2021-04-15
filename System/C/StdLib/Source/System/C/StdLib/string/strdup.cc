#include <System/C/StdLib/string.h>

#include <System/ABI/String/String.hh>

#include <System/C/StdLib/stdlib.h>


char* strdup(const char* str)
{
    if (!str)
        return nullptr;

    auto len = __strlen(str);
    if (len == 0)
        return nullptr;

    auto ptr = malloc(len + 1);
    if (!ptr)
        return nullptr;

    __memcpy(ptr, str, len + 1);

    return reinterpret_cast<char*>(ptr);
}

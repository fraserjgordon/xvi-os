#include <System/Unicode/UTF/UTF8.h>

#include <System/Unicode/UTF/UTF8Codec.hh>


namespace System::Unicode::UTF
{


bool IsValidUTF8ByteSequence(const char* ptr, size_t len)
{
    return UTF8Codec::isValid(ptr, len);
}


} // namespace System::Unicode::UTF

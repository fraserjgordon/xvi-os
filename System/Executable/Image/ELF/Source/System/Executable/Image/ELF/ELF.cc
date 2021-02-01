#include <System/Executable/Image/ELF/Types.hh>


namespace System::Executable::Image::ELF
{


auto foo(symbol_32be x)
{
    return System::Utility::Endian::byte_swap(x);
}


} // namespace System::Image::ELF

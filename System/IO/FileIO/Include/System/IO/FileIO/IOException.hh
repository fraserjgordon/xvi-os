#ifndef __SYSTEM_IO_FILEIO_IOEXCEPTION_H
#define __SYSTEM_IO_FILEIO_IOEXCEPTION_H


#include <stdexcept>
#include <system_error>


namespace System::IO::FileIO
{


class IOException : public std::runtime_error
{
public:

    IOException(std::error_code ec) :
        std::runtime_error(ec.message())
    {
    }

    IOException(int err) :
        IOException(std::error_code(err, std::generic_category()))
    {
    }
};


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_IOEXCEPTION_H */

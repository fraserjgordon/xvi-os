#ifndef __SYSTEM_IO_FILEIO_DIRECTORYHANDLE_H
#define __SYSTEM_IO_FILEIO_DIRECTORYHANDLE_H


#include <System/IO/FileIO/PathHandle.hh>


namespace System::IO::FileIO
{


class DirectoryHandle :
    public PathHandle
{
public:

    constexpr DirectoryHandle() noexcept = default;
    DirectoryHandle(const DirectoryHandle&) = delete;
    constexpr DirectoryHandle(DirectoryHandle&&) noexcept = default;

    ~DirectoryHandle() override = default;

    void operator=(const DirectoryHandle&) = delete;
    constexpr DirectoryHandle& operator=(DirectoryHandle&&) noexcept = default;
};


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_DIRECTORYHANDLE_H */

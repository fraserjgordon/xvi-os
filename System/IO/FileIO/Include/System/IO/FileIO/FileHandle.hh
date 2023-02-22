#ifndef __SYSTEM_IO_FILEIO_FILEHANDLE_H
#define __SYSTEM_IO_FILEIO_FILEHANDLE_H


#include <System/IO/FileIO/IOHandle.hh>
#include <System/IO/FileIO/PathHandle.hh>


namespace System::IO::FileIO
{


class FileHandle :
    public IOHandle
{
public:

    constexpr FileHandle() noexcept = default;
    FileHandle(const FileHandle&) = delete;
    constexpr FileHandle(FileHandle&&) noexcept = default;

    constexpr FileHandle(AbstractHandle h) noexcept :
        IOHandle(h)
    {
    }

    virtual ~FileHandle() = default;

    void operator=(const FileHandle&) = delete;
    FileHandle& operator=(FileHandle&&) = default;


    template <std::derived_from<FileHandle> T> FileHandle(T&&) = delete;
    template <std::derived_from<FileHandle> T> void operator=(T&&) = delete;    


    result<FileHandle> clone() const noexcept;


    static result<FileHandle> open(const PathHandle& at, const path_type& path, mode, creation, caching) noexcept;
};


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_FILEHANDLE_H */

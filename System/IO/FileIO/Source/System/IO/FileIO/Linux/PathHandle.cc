#include <System/IO/FileIO/PathHandle.hh>

#include <fcntl.h>
#include <unistd.h>

#include <System/IO/FileIO/IOException.hh>


namespace System::IO::FileIO
{


PathHandle::PathHandle(const std::filesystem::path& path) :
    PathHandle{}
{
    auto fd = ::open(path.c_str(), O_PATH|O_CLOEXEC);
    if (fd < 0)
        throw IOException(errno);

    m_handle = AbstractHandle{AbstractHandle::handle_type::fd_directory, fd, AbstractHandle::flag::none};
}


PathHandle::PathHandle(const PathHandle& at, const std::filesystem::path& path) :
    PathHandle{}
{
    auto fd = ::openat(at.handle().fd(), path.c_str(), O_PATH|O_CLOEXEC);
    if (fd < 0)
        throw IOException(errno);

    m_handle = AbstractHandle{AbstractHandle::handle_type::fd_directory, fd, AbstractHandle::flag::none};
}


PathHandle::PathHandle(int fd) noexcept :
    Handle{AbstractHandle{AbstractHandle::handle_type::fd, fd, AbstractHandle::flag::none}}
{
}


PathHandle::result<PathHandle> PathHandle::clone() const noexcept
{
    auto result = Handle::clone();
    if (!result)
        return std::unexpected(result.error());

    PathHandle handle;
    handle.m_handle = result->release();
    return handle;
}


PathHandle::result<PathHandle> PathHandle::open(const path_type& path) noexcept
{
    auto fd = ::open(path.c_str(), O_PATH|O_CLOEXEC);
    if (fd < 0)
        return errorCode(errno);

    return PathHandle{fd};
}


PathHandle::result<PathHandle> PathHandle::open(const PathHandle& at, const path_type& path) noexcept
{
    auto fd = ::openat(at.handle().fd(), path.c_str(), O_PATH|O_CLOEXEC);
    if (fd < 0)
        return errorCode(errno);

    return PathHandle{fd};
}


} // namespace System::IO::FileIO

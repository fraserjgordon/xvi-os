#include <System/IO/FileIO/Handle.hh>

#include <cstdio>

#include <unistd.h>

#include <System/IO/FileIO/IOException.hh>


namespace System::IO::FileIO
{


Handle::~Handle()
{
    // Terminate if closing fails.
    if (!tryClose())
        std::terminate();
}


void Handle::close()
{
    if (auto res = tryClose(); !res)
        throw IOException(res.error());
}


Handle::result<void> Handle::tryClose() noexcept
{
    if (!m_handle.isValid())
        return {};

    if (m_handle.isFd())
    {
        if (::close(m_handle.fd()) < 0) [[unlikely]]
            return std::unexpected(std::error_code(errno, std::generic_category()));

        return {};
    }

    return std::unexpected(std::make_error_code(std::errc::not_supported));
}


AbstractHandle Handle::release() noexcept
{
    return std::move(m_handle);
}


Handle::result<Handle::path_type> Handle::currentPath() const
{
    // We only know how to do this for FD-based handles.
    if (!m_handle.isFd())
        return errorCode(std::errc::not_supported);

    // Form the path to the special /proc entry for the descriptor.
    char proc[32];    // Enough for "/proc/self/fd/xxxxxxxxx".
    std::sprintf(proc, "/proc/self/fd/%i", m_handle.fd());

    // Get the filename corresponding to the FD, if we can.
    char path[PATH_MAX];
    auto len = ::readlink(proc, path, PATH_MAX); 
    if (len < 0)
        return errorCode(errno);

    return path_type(path, path + len);
}


Handle::result<Handle> Handle::clone() const noexcept
{
    // We only know how to clone FD-based handles.
    if (!m_handle.isFd())
        return errorCode(std::errc::not_supported);

    // Duplicate the file descriptor.
    int fd = ::dup(m_handle.fd());
    if (fd < 0)
        return errorCode(errno);

    auto handle = AbstractHandle{m_handle.type(), fd, m_handle.flags(), m_handle.info1(), m_handle.info2()};
    return Handle{handle};
}


} // namespace System::IO::FileIO

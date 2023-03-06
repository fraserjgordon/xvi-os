#ifndef __SYSTEM_IO_FILEIO_HANDLE_H
#define __SYSTEM_IO_FILEIO_HANDLE_H


#include <expected>
#include <filesystem>
#include <system_error>

#include <System/IO/FileIO/AbstractHandle.hh>
#include <System/IO/FileIO/SharedHandle.hh>
#include <System/IO/FileIO/UniqueHandle.hh>


namespace System::IO::FileIO
{


class Handle
{
public:

    using path_type = std::filesystem::path;

    template <class T> using result = FileIO::result<T>;


    constexpr Handle() = default;
    Handle(const Handle&) = delete;
    constexpr Handle(Handle&& x) = default;

    constexpr explicit Handle(AbstractHandle h) noexcept :
        m_handle(h)
    {
    }

    Handle& operator=(const Handle&) = delete;
    constexpr Handle& operator=(Handle&&) = default;

    template <std::derived_from<Handle> T> Handle(T&&) = delete;
    template <std::derived_from<Handle> T> void operator=(T&&) = delete;


    virtual ~Handle();

    virtual void close();

    virtual result<void> tryClose() noexcept;

    virtual result<path_type> currentPath() const;

    virtual AbstractHandle release() noexcept;

    result<Handle> clone() const noexcept;


    constexpr bool isValid() const noexcept
    {
        return m_handle.isValid();
    }

    constexpr bool isReadable() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::readable);
    }

    constexpr bool isWriteable() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::writeable);
    }

    constexpr bool isAppendOnly() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::append_only);
    }

    constexpr bool isNonblocking() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::nonblocking);
    }

    constexpr bool isAlignedIORequired() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::aligned_io);
    }

    constexpr bool isSeekable() const noexcept
    {
        return !!(m_handle.flags() & AbstractHandle::flag::seekable);
    }

    constexpr AbstractHandle handle() const noexcept
    {
        return m_handle;
    }

protected:

    AbstractHandle  m_handle = {};


    static auto errorCode(int err) noexcept
    {
        return std::unexpected(std::error_code(err, std::generic_category()));
    }

    static auto errorCode(std::errc err) noexcept
    {
        return std::unexpected(std::make_error_code(err));
    }
};


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_HANDLE_H */

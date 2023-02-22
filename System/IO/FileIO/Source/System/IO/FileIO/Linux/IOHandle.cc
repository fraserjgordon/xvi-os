#include <System/IO/FileIO/IOHandle.hh>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>


namespace System::IO::FileIO
{


IOHandle::result<IOHandle> IOHandle::clone() const noexcept
{
    // Clone as the base class.
    auto handle = Handle::clone();
    if (!handle)
        return std::unexpected(handle.error());

    // Move the contents of the cloned handle into an IOHandle.
    IOHandle ioh{};
    ioh.m_handle = handle->release();

    return ioh;
}


IOHandle::result<std::span<buffer>> IOHandle::read(offset_t offset, std::span<buffer> buffers) noexcept
{
    // The buffer type has the same layout as struct iovec.
    auto iovec = reinterpret_cast<const ::iovec*>(buffers.data());

    // Perform the read into the supplied buffers.
    auto result = ::preadv(m_handle.fd(), iovec, buffers.size(), offset);
    if (result < 0)
        return errorCode(errno);

    // Adjust the buffer list we were given to reflect how much data was actually read.
    auto remaining = static_cast<std::size_t>(result);
    std::size_t i = 0;
    for (; i < buffers.size(); ++i)
    {
        // How much of this buffer was written?
        auto& buf = buffers.data()[i];
        if (remaining >= buf.size())
        {
            // This entire buffer was written.
            remaining -= buf.size();
        }
        else
        {
            // Only part of this buffer was written. Adjust it.
            buf = buffer{buf.data(), remaining};
            remaining = 0;
        }

        // Skip the rest of the buffers if they weren't touched.
        if (remaining == 0)
            break;
    }

    // The returned span of buffers may be shorter than the input list if some were untouched.
    return buffers.subspan(0, i + 1);
}


IOHandle::result<std::span<const_buffer>> IOHandle::write(offset_t offset, std::span<const_buffer> buffers) noexcept
{
    // The buffer type has the same layout as struct iovec.
    auto iovec = reinterpret_cast<const ::iovec*>(buffers.data());

    // Perform the write into the supplied buffers.
    auto result = ::pwritev(m_handle.fd(), iovec, buffers.size(), offset);
    if (result < 0)
        return errorCode(errno);

    // Adjust the buffer list we were given to reflect how much data was actually written.
    auto remaining = static_cast<std::size_t>(result);
    std::size_t i = 0;
    for (; i < buffers.size(); ++i)
    {
        // How much of this buffer was written?
        auto& buf = buffers.data()[i];
        if (remaining >= buf.size())
        {
            // This entire buffer was written.
            remaining -= buf.size();
        }
        else
        {
            // Only part of this buffer was written. Adjust it.
            buf = const_buffer{buf.data(), remaining};
            remaining = 0;
        }

        // Skip the rest of the buffers if they weren't touched.
        if (remaining == 0)
            break;
    }

    // The returned span of buffers may be shorter than the input list if some were untouched.
    return buffers.subspan(0, i + 1);
}


IOHandle::result<std::span<buffer>> IOHandle::write(offset_t offset, std::span<buffer> buffers) noexcept
{
    // Just cast our way through.
    auto const_buffers = std::span{reinterpret_cast<const_buffer*>(buffers.data()), buffers.size()};
    auto res = write(offset, const_buffers);
    if (!res)
        return std::unexpected(res.error());

    return std::span{reinterpret_cast<buffer*>(res->data()), res->size()};
}


std::size_t IOHandle::maxBufferCount() const noexcept
{
    return IOV_MAX;
}


IOHandle::result<offset_t> IOHandle::size() const noexcept
{
    // We only want the size.
    struct ::statx output;
    auto res = ::statx(m_handle.fd(), "", AT_EMPTY_PATH, STATX_SIZE, &output);

    if (res < 0)
        return errorCode(errno);

    // Did we actually get a size?
    if (!(output.stx_mask & STATX_SIZE)) [[unlikely]]
        return errorCode(std::errc::not_supported);

    return output.stx_size;
}


} // namespace System::IO::FileIO

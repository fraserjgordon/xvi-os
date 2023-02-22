#include <System/IO/FileIO/FileHandle.hh>

#include <fcntl.h>


namespace System::IO::FileIO
{


FileHandle::result<FileHandle> FileHandle::open(const PathHandle& at, const path_type& path, mode open_mode, creation create, caching cache) noexcept
{
    // Flags we're going to pass to the syscall.
    int options = O_CLOEXEC;
    int mode = 0;

    // Our internal flags.
    using flag = AbstractHandle::flag;
    flag flags = flag::none;

    switch (open_mode)
    {
        case mode::none:
            options |= O_PATH;
            break;

        case mode::metadata_read:
        case mode::metadata_write:
        case mode::read:
            options |= O_RDONLY;
            flags |= flag::seekable|flag::readable;
            break;

        case mode::write:
            options |= O_RDWR;
            flags |= flag::seekable|flag::readable|flag::writeable;
            break;

        case mode::append:
            options |= O_WRONLY|O_APPEND;
            flags |= flag::writeable|flag::append_only;
            break;

        case mode::unchanged:
        default:
            return errorCode(std::errc::invalid_argument);
    }

    switch (create)
    {
        case creation::open_existing:
            break;

        case creation::only_new:
            options |= O_CREAT|O_EXCL;
            mode = 0777;
            break;

        case creation::if_needed:
            options |= O_CREAT;
            mode = 0777;
            break;

        case creation::truncate_existing:
            options |= O_TRUNC;
            break;

        case creation::always_new:
            // Not implemented.
            return errorCode(std::errc::not_supported);

        default:
            return errorCode(std::errc::invalid_argument);
    }

    switch (cache)
    {
        case caching::none:
            options |= O_SYNC|O_DIRECT;
            flags |= flag::aligned_io;
            break;

        case caching::metadata_only:
            options |= O_DIRECT;
            flags |= flag::aligned_io|flag::cache_metadata;
            break;

        case caching::read:
            options |= O_SYNC;
            flags |= flag::cache_reads|flag::cache_needs_fsync;
            break;

        case caching::read_and_metadata:
            options |= O_DSYNC;
            flags |= flag::cache_reads|flag::cache_metadata|flag::cache_needs_fsync;
            break;

        case caching::all:
            flags |= flag::cache_reads|flag::cache_metadata|flag::cache_writes;
            break;

        case caching::all_with_safety_barriers:
            flags |= flag::cache_reads|flag::cache_metadata|flag::cache_writes|flag::cache_needs_fsync;
            break;

        case caching::unchanged:
        default:
            return errorCode(std::errc::invalid_argument);
    }

    // Open the file, using the root if it was given.
    int fd = at.isValid() ? at.handle().fd() : AT_FDCWD;
    int res = ::openat(fd, path.c_str(), options, mode);

    if (res < 0)
        return errorCode(errno);

    return FileHandle{AbstractHandle{AbstractHandle::handle_type::fd_file, res, flags}};
}


} // namespace System::IO::FileIO

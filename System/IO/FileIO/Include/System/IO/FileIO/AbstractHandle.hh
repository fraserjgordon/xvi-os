#ifndef __SYSTEM_IO_FILEIO_ABSTRACTHANDLE_H
#define __SYSTEM_IO_FILEIO_ABSTRACTHANDLE_H


#include <compare>
#include <cstdint>
#include <utility>

#include <System/IO/FileIO/Types.hh>


namespace System::IO::FileIO
{


class AbstractHandle
{
public:

    enum class handle_type : std::uint8_t
    {
        // Stored in u_storage.
        invalid = 0,

        // Stored in u_fd.
        fd,
        fd_file,
        fd_directory,

        // Stored in u_ptr.
        ptr_mmap,
        ptr_user,
    };

    friend constexpr auto operator<=>(handle_type x, handle_type y) noexcept
    {
        using T = std::underlying_type_t<handle_type>;
        return static_cast<T>(x) <=> static_cast<T>(y);
    }


    enum class flag : std::uint16_t
    {
        none                = 0,

        // Permissions.
        readable            = 0x0001,
        writeable           = 0x0002,
        append_only         = 0x0004,

        // I/O properties.
        nonblocking         = 0x0008,
        aligned_io          = 0x0010,
        seekable            = 0x0020,

        // Caching & flushing.
        cache_reads         = 0x1000,
        cache_writes        = 0x2000,
        cache_metadata      = 0x4000,
        cache_needs_fsync   = 0x8000,
    };

    friend constexpr flag operator& (flag x, flag y) noexcept
    {
        using T = std::underlying_type_t<flag>;
        return static_cast<flag>(static_cast<T>(x) & static_cast<T>(y));
    }

    friend constexpr flag operator| (flag x, flag y) noexcept
    {
        using T = std::underlying_type_t<flag>;
        return static_cast<flag>(static_cast<T>(x) | static_cast<T>(y));
    }

    friend constexpr flag operator~ (flag x) noexcept
    {
        using T = std::underlying_type_t<flag>;
        return static_cast<flag>(~static_cast<T>(x));
    }

    friend constexpr flag& operator&= (flag& x, flag y) noexcept
    {
        x = x & y;
        return x;
    }

    friend constexpr flag& operator|= (flag& x, flag y) noexcept
    {
        x = x | y;
        return x;
    }

    friend constexpr bool operator! (flag x) noexcept
    {
        return x != flag::none;
    }


    constexpr AbstractHandle() = default;
    constexpr AbstractHandle(const AbstractHandle&) = default;

    constexpr AbstractHandle(AbstractHandle&& other) :
        m_handle{std::exchange(other.m_handle, storage_t{})},
        m_type{std::exchange(other.m_type, handle_type::invalid)},
        m_info1{std::exchange(other.m_info1, 0)},
        m_flags{std::exchange(other.m_flags, flag::none)},
        m_info2{std::exchange(other.m_info2, 0)}
    {
    }

    constexpr AbstractHandle(handle_type type, int fd, flag flags, std::uint8_t info1 = 0, std::uint32_t info2 = 0) :
        m_handle{.u_fd = fd},
        m_type{type},
        m_info1{info1},
        m_flags{flags},
        m_info2{info2}
    {
    }

    // Note: does not close the handle on destruction!
    constexpr ~AbstractHandle() = default;

    constexpr AbstractHandle& operator=(const AbstractHandle&) = default;

    constexpr AbstractHandle& operator=(AbstractHandle&& other)
    {
        m_handle = std::exchange(other.m_handle, storage_t{});
        m_type = std::exchange(other.m_type, handle_type::invalid);
        m_info1 = std::exchange(other.m_info1, 0);
        m_flags = std::exchange(other.m_flags, flag::none);
        m_info2 = std::exchange(other.m_info2, 0);

        return *this;
    }

    void swap(AbstractHandle& other)
    {
        using std::swap;
        swap(m_handle, other.m_handle);
        swap(m_type, other.m_type);
        swap(m_info1, other.m_info1);
        swap(m_flags, other.m_flags);
        swap(m_info2, other.m_info2);
    }

    constexpr int fd() const noexcept
    {
        return m_handle.u_fd;
    }

    constexpr void* ptr() const noexcept
    {
        return m_handle.u_ptr;
    }

    constexpr handle_type type() const noexcept
    {
        return m_type;
    }

    constexpr flag flags() const noexcept
    {
        return m_flags;
    }

    constexpr auto info1() const noexcept
    {
        return m_info1;
    }

    constexpr auto info2() const noexcept
    {
        return m_info2;
    }

    constexpr bool isValid() const noexcept
    {
        return type() != handle_type::invalid;
    }

    constexpr bool isPointer() const noexcept
    {
        auto t = type();
        return (t >= handle_type::ptr_mmap && t <= handle_type::ptr_user);
    }

    constexpr bool isFd() const noexcept
    {
        auto t = type();
        return (t >= handle_type::fd && t <= handle_type::fd_directory);
    }

private:

    static constexpr std::intptr_t InvalidHandle = -1;

    union storage_t
    {
        std::intptr_t       u_storage = InvalidHandle;
        int                 u_fd;
        void*               u_ptr;
    };


    storage_t               m_handle = {};
    handle_type             m_type = handle_type::invalid;
    std::uint8_t            m_info1 = 0;
    flag                    m_flags = flag::none; 
    std::uint32_t           m_info2 = 0;
};


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_ABSTRACTHANDLE_H */

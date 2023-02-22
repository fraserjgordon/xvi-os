#ifndef __SYSTEM_IO_FILEIO_TYPES_H
#define __SYSTEM_IO_FILEIO_TYPES_H


#include <cstddef>
#include <expected>
#include <memory>
#include <span>


namespace System::IO::FileIO
{


using offset_t = std::int64_t;


enum class mode : std::uint8_t
{
    unchanged,
    none,
    metadata_read,
    metadata_write,
    read,
    write,
    append,
};

enum class creation : std::uint8_t
{
    open_existing,
    only_new,
    if_needed,
    truncate_existing,
    always_new,
};

enum class caching : std::uint8_t
{
    unchanged,
    none,
    metadata_only,
    read,
    read_and_metadata,
    all,
    all_with_safety_barriers,
    temporary,
};


struct buffer_storage_t
{
    std::byte*          data = nullptr;
    std::size_t         size = 0;
};

struct const_buffer_storage_t
{
    const std::byte*    data = nullptr;
    std::size_t         size = 0;
};


class buffer
{
public:

    using pointer = std::byte*;
    using const_pointer = const std::byte*;
    using size_type = std::size_t;
    using iterator = std::byte*;
    using const_iterator = const std::byte*;

    constexpr buffer() noexcept = default;
    constexpr buffer(const buffer&) noexcept = default;

    constexpr buffer(pointer data, size_type len) noexcept :
        m_iovec{data, len}
    {
    }

    constexpr buffer(std::span<std::byte> span) noexcept :
        m_iovec{span.data(), span.size()}
    {
    }

    constexpr ~buffer() = default;

    constexpr buffer& operator=(const buffer&) noexcept = default;


    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr pointer data() noexcept
    {
        return m_iovec.data;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_iovec.data;
    }

    constexpr size_type size() const noexcept
    {
        return m_iovec.size;
    }

    constexpr pointer begin() noexcept
    {
        return data();
    }

    constexpr const_pointer begin() const noexcept
    {
        return data();
    }

    constexpr pointer end() noexcept
    {
        return data() + size();
    }

    constexpr const_pointer end() const noexcept
    {
        return data() + size();
    }

    constexpr const_pointer cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_pointer cend() const noexcept
    {
        return end();
    }

private:

    buffer_storage_t    m_iovec = {};
};


class const_buffer
{
public:

    using pointer = const std::byte*;
    using const_pointer = const std::byte*;
    using size_type = std::size_t;
    using iterator = const std::byte*;
    using const_iterator = const std::byte*;

    constexpr const_buffer() noexcept = default;
    constexpr const_buffer(const const_buffer&) noexcept = default;

    constexpr const_buffer(const_pointer data, size_type len) noexcept :
        m_iovec{data, len}
    {
    }

    constexpr const_buffer(std::span<std::byte> span) noexcept :
        m_iovec{span.data(), span.size()}
    {
    }

    constexpr const_buffer(std::span<const std::byte> span) noexcept :
        m_iovec{span.data(), span.size()}
    {
    }

    constexpr const_buffer(buffer buf) noexcept :
        m_iovec{buf.data(), buf.size()}
    {
    }

    constexpr ~const_buffer() = default;

    constexpr const_buffer& operator=(const const_buffer&) noexcept = default;


    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_iovec.data;
    }

    constexpr size_type size() const noexcept
    {
        return m_iovec.size;
    }

    constexpr const_pointer begin() const noexcept
    {
        return data();
    }

    constexpr const_pointer end() const noexcept
    {
        return data() + size();
    }

    constexpr const_pointer cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_pointer cend() const noexcept
    {
        return end();
    }

private:

   const_buffer_storage_t   m_iovec = {};
};


struct extent
{
    offset_t        offset;
    std::size_t     length;
};


template <class T> using result = std::expected<T, std::error_code>;


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_TYPES_H */

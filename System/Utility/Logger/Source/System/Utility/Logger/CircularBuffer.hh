#ifndef __SYSTEM_UTILITY_LOGGER_CIRCULARBUFFER_H
#define __SYSTEM_UTILITY_LOGGER_CIRCULARBUFFER_H


#include <mutex>
#include <span>

#include <System/Utility/Logger/MessageInternal.hh>


namespace System::Utility::Logger
{


class circular_buffer
{
public:

    circular_buffer() noexcept = default;

    circular_buffer(const circular_buffer&) = delete;
    circular_buffer(circular_buffer&&) = delete;

    circular_buffer& operator=(const circular_buffer&) = delete;
    circular_buffer& operator=(circular_buffer&&) = delete;

    circular_buffer(std::span<std::byte> buffer) :
        m_buffer(buffer)
    {
    }

    std::size_t size() const noexcept
    {
        // No locking: size is fixed one created.
        return m_buffer.size();
    }

    std::size_t maxCapacity() const noexcept
    {
        // One byte is unusable to prevent the write pointer from lapping read.
        return size() - 1;
    }

    std::size_t usedCapacity() const noexcept
    {
        auto guard = std::lock_guard(m_mutex);

        return usedCapacityUnlocked();
    }

    std::size_t availableCapacity() const noexcept
    {
        return maxCapacity() - usedCapacity();
    }

    bool add(std::span<const std::byte> in)
    {
        auto guard = std::lock_guard(m_mutex);

        // Can we fit the new message without discarding?
        auto size = in.size();
        size = std::min(size, message_internal::encodedMessageSize(in));
        if (size > maxCapacity())
        {
            // We'll never be able to fit this message.
            return false;
        }
        else if (size > availableCapacityUnlocked())
        {
            // Discard messages until we have enough room.
            while (size > availableCapacityUnlocked())
            {
                auto message_size = peekNextMessageSize();
                if (message_size == 0)
                {
                    // Buffer corruption... shouldn't have happened. Recover by discarding all contents.
                    m_read = m_write;
                    break;
                }
                
                // Skip this message.
                advance(m_read, message_size);
            }
        }

        // Insert the new message.
        copyFrom(size, in.data());
        advance(m_write, size);

        return true;
    }

    std::size_t extractTo(std::ptrdiff_t offset, std::span<std::byte> out, bool remove)
    {
        if (remove && offset != 0)
            return 0;
        
        auto guard = std::lock_guard(m_mutex);

        auto original_read = m_read;
        advance(m_read, offset);

        auto size = peekNextMessageSize();

        if (size == 0)
        {
            m_read = original_read;
            return 0;
        }
        else if (size > out.size())
        {
            m_read = original_read;
            return size;
        }

        copyTo(size, out.data());

        if (remove)
            advance(m_read, size);
        else
            m_read = original_read;

        return size;
    }

private:

    mutable std::mutex      m_mutex     = {};

    std::span<std::byte>    m_buffer    = {};
    std::size_t             m_read      = 0;
    std::size_t             m_write     = 0;


    std::size_t usedCapacityUnlocked() const noexcept
    {
        if (m_write >= m_read)
            return (m_write - m_read);
        else
            return size() - (m_read - m_write);
    }

    std::size_t availableCapacityUnlocked() const noexcept
    {
        return maxCapacity() - usedCapacityUnlocked();
    }

    const std::byte* readPtr() const noexcept
    {
        return m_buffer.data() + m_read;
    }

    std::byte* writePtr() noexcept
    {
        return m_buffer.data() + m_write;
    }

    void advance(std::size_t& ptr, std::size_t n)
    {
        ptr = (ptr + n) % size();
    }

    bool copyTo(std::size_t n, std::byte* ptr) const
    {
        // Assumes: m_mutex is held by the caller.

        if (usedCapacityUnlocked() < n)
            return false;

        auto copy_one = std::min(n, size() - m_read);
        auto copy_two = (copy_one < n) ? n - copy_one : 0;

        std::copy_n(readPtr(), copy_one, ptr);
        std::copy_n(m_buffer.data(), copy_two, ptr + copy_one);
        return true;
    }

    void copyFrom(std::size_t n, const std::byte* ptr)
    {
        // Assumes: m_mutex is held by the caller.
        
        auto copy_one = std::min(n, size() - m_write);
        auto copy_two = (copy_one < n) ? n - copy_one : 0;

        std::copy_n(ptr, copy_one, writePtr());
        std::copy_n(ptr + copy_one, copy_two, m_buffer.data());
    }

    std::size_t peekNextMessageSize() const noexcept
    {
        // Assumes: m_mutex is held by caller.

        if (usedCapacityUnlocked() < 4)
            return 0;

        std::array<std::byte, 4> data;
        if (!copyTo(data.size(), &data[0]))
            return 0;

        auto size = message_internal::encodedMessageSize(data);
        if (size > usedCapacityUnlocked())
            return 0;

        return size;
    }
};


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_CIRCULARBUFFER_H */

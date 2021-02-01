#ifndef __SYSTEM_UTILITY_CPPEXTRA_BYTEBUFFER_H
#define __SYSTEM_UTILITY_CPPEXTRA_BYTEBUFFER_H


#include <array>
#include <bit>
#include <ranges>
#include <stdexcept>

#include <System/Utility/Endian/Endian.hh>


namespace System::Utility::CppExtra
{


using namespace System::Utility::Endian;


class byte_buffer_base
{
public:

    enum class whence
    {
        begin,
        end,
        current,
    };
};

template <std::ranges::viewable_range R>
    requires std::ranges::input_range<R>
        && std::is_same_v<std::byte, std::remove_cv_t<std::ranges::range_value_t<R>>>
class input_byte_buffer :
    public byte_buffer_base
{
public:

    using range             = R;
    using iterator          = std::ranges::iterator_t<R>;
    using difference_type   = std::ranges::range_difference_t<R>;
    using size_type         = std::make_unsigned_t<difference_type>;

    constexpr input_byte_buffer() = default;

    constexpr input_byte_buffer(R input, std::endian endian = std::endian::native) noexcept :
        m_range(input),
        m_pos(std::ranges::begin(input)),
        m_endian(endian)
    {
    }

    constexpr input_byte_buffer(R input, whence w, std::endian endian = std::endian::native)
        requires std::ranges::random_access_range<R>
        : input_byte_buffer(input, endian)
    {
        switch (w)
        {
            case whence::begin:
            case whence::current:
                m_pos = std::ranges::begin(input);
                break;

            case whence::end:
                m_pos = std::ranges::end(input);
                break;
        }
    }

    constexpr std::endian endian() const noexcept
    {
        return m_endian;
    }

    constexpr void set_endian(std::endian e) noexcept
    {
        m_endian = e;
    }

    constexpr size_type size() const noexcept
        requires std::ranges::sized_range<R>
    {
        return std::ranges::size(m_range);
    }

    constexpr size_type remaining() const noexcept
        requires std::ranges::sized_range<R>
    {
        return static_cast<size_type>(std::ranges::end(m_range) - m_pos);
    }

    constexpr difference_type tell() const noexcept
        requires std::ranges::random_access_range<R>
    {
        return m_pos - std::ranges::begin(m_range);
    }

    constexpr void seek(difference_type pos, whence w = whence::current)
        requires std::ranges::random_access_range<R>
    {
        switch (w)
        {
            case whence::end:
                pos = static_cast<difference_type>(size()) - pos;
                break;

            case whence::current:
                pos += tell();
                break;

            case whence::begin:
                break;
        }

        // Deliberately not '>= size()' because seeking to the end is allowed.
        if (pos < 0 || static_cast<size_type>(pos) > size())
            throw std::out_of_range("invalid seek position");

        m_pos = std::ranges::begin(m_range) + pos;
    }

    constexpr std::span<const std::byte> readData(size_type n)
        requires std::ranges::random_access_range<R>
    {
        if (n > remaining())
            throw std::out_of_range("buffer underrun");

        std::span span{std::addressof(*m_pos), n};
        m_pos += n;

        return span;
    }

    constexpr void readDataTo(size_type n, std::span<std::byte> data)
    {
        if constexpr (std::ranges::sized_range<R>)
        {
            std::ranges::copy(readData(n), data.data());
        }
        else
        {
            for (auto i = data.begin(); i != data.end(); ++i)
            {
                if (m_pos == std::ranges::end(m_range))
                    throw std::out_of_range("buffer underrun");

                *i = *m_pos;
                ++m_pos;
            }
        }
    }

    template <class I>
        requires (is_endian_type_v<I>)
    constexpr typename I::underlying_native_endian_type read()
    {
        std::array<std::byte, sizeof(I)> data;
        readDataTo(sizeof(I), data);
        return std::bit_cast<I>(data);
    }

    template <class I>
        requires (std::is_integral_v<I> && sizeof(I) > 1)
    constexpr I read()
    {
        if (m_endian == std::endian::little)
            return read<little_endian_type<I>>();
        else
            return read<big_endian_type<I>>();
    }

    template <class I>
    constexpr std::byte read()
        requires (std::is_same_v<I, std::byte>)
    {
        return readData(1)[0];
    }

    template <class I>
    constexpr I read()
        requires (std::is_same_v<I, std::uint8_t> || std::is_same_v<I, std::int8_t>)
    {
        return static_cast<I>(read<std::byte>());
    }

    template <class T>
    const T* ptr() const noexcept
        requires std::ranges::contiguous_range<R>
    {
        return reinterpret_cast<const T*>(std::addressof(*m_pos));
    }

private:

    R               m_range;
    iterator        m_pos;
    std::endian     m_endian;
};

template <std::ranges::viewable_range R>
input_byte_buffer(R&&) -> input_byte_buffer<std::views::all_t<R>>;


template <class R>
concept growable_buffer_range = requires(R& r, std::byte b)
{
    { r.push_back(b) };
};

template <class R, bool Growing = growable_buffer_range<R>>
    requires (std::ranges::viewable_range<R>
        && std::ranges::output_range<R, std::byte>
        && (!Growing || growable_buffer_range<R>))
class output_byte_buffer :
    public byte_buffer_base
{
public:

    using range             = R;
    using view              = std::views::all_t<R>;
    using iterator          = std::ranges::iterator_t<R>;
    using difference_type   = std::ranges::range_difference_t<range>;
    using size_type         = std::make_unsigned_t<difference_type>;

    static constexpr bool can_reserve = Growing && requires(R& r, size_type sz)
    {
        { r.reserve(sz) } ;
    };

    constexpr output_byte_buffer() = default;

    constexpr output_byte_buffer(view output)
        requires (!Growing)
        : m_range(output),
          m_pos(std::ranges::begin(output))
    {
    }

    constexpr output_byte_buffer(R& output, std::endian endian = std::endian::native)
        requires (Growing)
        : m_range(output),
          m_pos(std::ranges::begin(output)),
          m_endian(endian)
    {
    }

    constexpr output_byte_buffer(R& output, whence w, std::endian endian = std::endian::native)
        requires (Growing)
        : m_range(output),
          m_pos(),
          m_endian(endian)
    {
        switch (w)
        {
            case whence::begin:
                m_pos = std::ranges::begin(m_range);
                break;

            case whence::current:
            case whence::end:
                m_pos = std::ranges::end(m_range);
                break;
        }
    }

    constexpr std::endian endian() const noexcept
    {
        return m_endian;
    }

    constexpr void set_endian(std::endian e) noexcept
    {
        m_endian = e;
    }

    constexpr size_type size() const noexcept
        requires std::ranges::sized_range<R>
    {
        return std::ranges::size(m_range);
    }

    constexpr difference_type tell() const noexcept
        requires std::ranges::random_access_range<R>
    {
        return m_pos - std::ranges::begin(m_range);
    }

    constexpr void seek(difference_type pos, whence w)
    {
        switch (w)
        {
            case whence::end:
                pos = static_cast<difference_type>(size()) - pos;
                break;

            case whence::current:
                pos += tell();
                break;

            case whence::begin:
                break;
        }

        // Deliberately not '>= size()' because seeking to the end is allowed.
        if (pos < 0 || static_cast<size_type>(pos) > size())
            throw std::out_of_range("invalid seek position");

        m_pos = std::ranges::begin(m_range) + pos;
    }

    constexpr void writeData(std::span<const std::byte> data)
    {
        if constexpr (can_reserve)
        {
            auto current = tell();
            auto write_end = static_cast<size_type>(current) + data.size();
            if (write_end > size())
            {
                m_range.resize(write_end);
                m_pos = std::ranges::begin(m_range) + current;
            }
        }

        for (auto b : data)
        {
            if (m_pos == std::ranges::end(m_range))
            {
                if constexpr (!Growing)
                    throw std::out_of_range("buffer overrun");

                m_range.push_back(b);
                ++m_pos;
            }
            else
            {
                *m_pos++ = b;
            }
        }
    }
    
    template <class I, class J>
        requires (is_endian_type_v<I> && std::is_convertible_v<J, typename I::underlying_native_endian_type>)
    constexpr void write(J i)
    {
        using raw_t = std::array<std::byte, sizeof(I)>;
        raw_t raw = std::bit_cast<raw_t>(I(i));

        writeData(std::span(raw));
    }

    template <class I, class J>
        requires (std::is_integral_v<I> && sizeof(I) > 1 && std::is_convertible_v<J, I>)
    constexpr void write(J i)
    {
        if (m_endian == std::endian::little)
            write<little_endian_type<I>>(i);
        else
            write<big_endian_type<I>>(i);
    }

    template <class I>
        requires (std::is_same_v<I, std::byte>)
    constexpr void write(std::type_identity_t<I> byte)
    {
        writeData({&byte, 1});
    }

    template <class I>
        requires (std::is_same_v<I, std::uint8_t> || std::is_same_v<I, std::int8_t>)
    constexpr void write(std::type_identity_t<I> value)
    {
        write<std::byte>(static_cast<std::byte>(value));
    }

private:

    using storage_type = std::conditional_t<Growing, R&, std::conditional_t<std::ranges::viewable_range<R>, R, std::views::all_t<R>>>;

    storage_type    m_range;
    iterator        m_pos;
    std::endian     m_endian    = std::endian::native;

    void advance(size_type n)
    {
        if constexpr (std::ranges::sized_range<R>)
        {
            if (std::distance(m_pos, std::ranges::end(m_range)) < n)
                throw std::out_of_range("can't advance beyond end of buffer");

            std::advance(m_pos, n);
        }
        else
        {
            for (; n > 0; --n)
            {
                if (m_pos == std::ranges::end(m_range))
                    throw std::out_of_range("can't advance beyond end of buffer");
                
                ++m_pos;
            }
        }
    }
};


template <class R>
output_byte_buffer(R&&) -> output_byte_buffer<std::views::all_t<R>>;

template <class R>
    requires (growable_buffer_range<R>)
output_byte_buffer(R&) -> output_byte_buffer<R>;


} // namespace System::Utility::CppExtra


#endif /* ifndef __SYSTEM_UTILITY_CPPEXTRA_BYTEBUFFER_H */

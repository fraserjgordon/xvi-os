#ifndef __SYSTEM_UTILITY_LOGGER_MESSAGEINTERNAL_H
#define __SYSTEM_UTILITY_LOGGER_MESSAGEINTERNAL_H


#include <atomic>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include <System/Utility/CppExtra/ByteBuffer.hh>
#include <System/Utility/CppExtra/RefcountedPtr.hh>

#include <System/Utility/Logger/Message.hh>


namespace System::Utility::Logger
{


class message_internal
    : public System::Utility::CppExtra::refcounted_mixin
{
public:

    using pointer = System::Utility::CppExtra::refcounted_ptr<message_internal>;
    using const_pointer = System::Utility::CppExtra::refcounted_ptr<const message_internal>;

    // Encoding formats.
    enum class encoding_format : std::uint32_t
    {
        experimental    = 0,    // Initial experimental encoding.

        current = experimental,
    };

    message_internal() = delete;
    message_internal(const message&) = delete;
    message_internal(message&&) = delete;
    message_internal& operator=(const message&) = delete;
    message_internal& operator=(message&&) = delete;

    void addVariable(std::string_view name, var_type type, std::span<const std::byte> data);
    void addVariable(std::string_view name, std::string_view value);
    void addVariable(std::string_view name, std::int64_t value);
    void addVariable(std::string_view name, std::uint64_t value);
    void addVariable(std::string_view name, std::span<const std::byte> binary);

    template <class I>
        requires (std::is_integral_v<I> && std::is_signed_v<I>)
    void addVariable(std::string_view name, I i)
    {
        addVariable(name, static_cast<std::int64_t>(i));
    }

    template <class I>
        requires (std::is_integral_v<I> && std::is_unsigned_v<I>)
    void addVariable(std::string_view name, I i)
    {
        addVariable(name, static_cast<std::uint64_t>(i));
    }

    std::string_view str() const noexcept
    {
        return m_msg;
    }

    opaque_message opaque() const noexcept
    {
        return reinterpret_cast<opaque_message>(this);
    }

    std::span<const std::byte> data() const noexcept
    {
        return m_preEncoded;
    }

    priority getPriority() const noexcept;

    // Completes encoding of a message. No further modifications should be made after this call.
    void finalise() noexcept;

    void ensureDecoded() const;


    static const_pointer fromOpaque(opaque_message om) noexcept
    {
        return const_pointer(reinterpret_cast<const message_internal*>(om));
    }

    static pointer create(std::string_view facility, priority p, std::string_view message)
    {
        return pointer(new message_internal(facility, p, message));
    }

    static const_pointer create(std::span<const std::byte> encoded)
    {
        return const_pointer(new message_internal(encoded));
    }

    static std::size_t encodedMessageSize(std::span<const std::byte> encoded);

private:

    friend class System::Utility::CppExtra::refcounted_ptr_traits<message_internal>;

    using input_buffer  = System::Utility::CppExtra::input_byte_buffer<std::span<const std::byte>>;
    using output_buffer = System::Utility::CppExtra::output_byte_buffer<std::vector<std::byte>>;

    struct variable_t
    {
        std::string_view            name;
        var_type                    type;
        std::span<const std::byte>  data;
    };


    std::string_view                    m_msg           = {};
    std::vector<variable_t>             m_vars          = {};

    std::vector<std::byte>              m_encoded       = {};
    std::span<const std::byte>          m_preEncoded    = {};


    message_internal(std::string_view facility, priority p, std::string_view message);
    message_internal(std::span<const std::byte> encoded);

    ~message_internal();

    output_buffer output() noexcept;

    bool decode();


    // Writes an integer using a variable-length encoding (identical to ULEB128).
    static void writeVarInt(output_buffer& buffer, std::uint64_t value);
    std::optional<std::uint64_t> readVarInt(input_buffer& buffer);
    static std::size_t varIntLength(std::uint64_t value);
};


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_MESSAGEINTERNAL_H */

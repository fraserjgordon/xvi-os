#include <System/Utility/Logger/Message.hh>
#include <System/Utility/Logger/MessageInternal.hh>

#include <algorithm>
#include <limits>

#include <System/Utility/CppExtra/ByteBuffer.hh>


namespace System::Utility::Logger
{


using uint32le_t = System::Utility::CppExtra::uint32le_t;


void freeMessage(opaque_message msg)
{
    static_cast<void>(message_internal::fromOpaque(msg));
}

std::pair<const char*, std::size_t> getMessageString(opaque_message msg)
{
    auto p = message_internal::fromOpaque(msg).take();
    p->ensureDecoded();
    auto sv = p->str();
    return {sv.data(), sv.length()};
}

priority getMessagePriority(opaque_message msg)
{
    auto p = message_internal::fromOpaque(msg).take();
    p->ensureDecoded();
    return p->getPriority();
}

opaque_message decodeMessage(const std::byte* buffer, std::size_t size)
{
    return message_internal::create({buffer, size}).take()->opaque();
}


message_internal::message_internal(std::string_view facility, priority p, std::string_view message)
{
    // Reserve space in the buffer for the message size. It comes before the encoding version so that messages with
    // unknown encodings can be skipped. It does, however, add the restriction that the size field cannot be changed in
    // any future encoding revisions.
    auto buffer = output();
    buffer.write<uint32le_t>(0U);

    // Write the version of the encoding that we're using.
    writeVarInt(buffer, static_cast<std::underlying_type_t<encoding_format>>(encoding_format::current));

    addVariable(SystemVariable::Facility, facility);
    addVariable(SystemVariable::Priority, static_cast<std::underlying_type_t<priority>>(p));
    addVariable(SystemVariable::Message, message);
}

message_internal::message_internal(std::span<const std::byte> buffer)
{
    m_preEncoded = buffer;
    ensureDecoded();
}

message_internal::~message_internal()
{
}

void message_internal::addVariable(std::string_view name, var_type type, std::span<const std::byte> value)
{
    // UVarInt      variable_length
    // UVarInt      type
    // UVarInt      name length
    // byte[...]    name
    // byte[...]    value
    auto name_length = static_cast<std::uint32_t>(name.length());
    auto data_length = static_cast<std::uint32_t>(value.size());
    auto name_bytes = std::span{reinterpret_cast<const std::byte*>(name.data()), name_length};
    
    // The variable length is excluding itself to avoid a circular dependency. It's possible to resolve that dependency
    // in a consistent manner but it complicates the code for no gain.
    auto type_length = varIntLength(static_cast<std::uint32_t>(type));
    auto name_length_length = varIntLength(name_length);
    auto content_length = type_length + name_length_length + name_length + data_length;

    auto buffer = output();
    writeVarInt(buffer, content_length);
    writeVarInt(buffer, static_cast<std::uint32_t>(type));
    writeVarInt(buffer, name_length);
    buffer.writeData(name_bytes);
    buffer.writeData(value);
}

void message_internal::addVariable(std::string_view name, std::string_view value)
{
    std::span as_span{reinterpret_cast<const std::byte*>(value.data()), value.length()};
    addVariable(name, var_type::string, as_span);
}

void message_internal::addVariable(std::string_view name, std::int64_t value)
{
    if (auto i = static_cast<std::int8_t>(value); i == value)
        addVariable(name, var_type::signed_integer, {reinterpret_cast<const std::byte*>(&i), 1});
    else if (auto j = static_cast<std::int16_t>(value); j == value)
        addVariable(name, var_type::signed_integer, {reinterpret_cast<const std::byte*>(&j), 2});
    else if (auto k = static_cast<std::int32_t>(value); k == value)
        addVariable(name, var_type::signed_integer, {reinterpret_cast<const std::byte*>(&k), 4});
    else
        addVariable(name, var_type::signed_integer, {reinterpret_cast<const std::byte*>(&value), 8});
}

void message_internal::addVariable(std::string_view name, std::uint64_t value)
{
    if (auto i = static_cast<std::uint8_t>(value); i == value)
        addVariable(name, var_type::unsigned_integer, {reinterpret_cast<const std::byte*>(&i), 1});
    else if (auto j = static_cast<std::uint16_t>(value); j == value)
        addVariable(name, var_type::unsigned_integer, {reinterpret_cast<const std::byte*>(&j), 2});
    else if (auto k = static_cast<std::uint32_t>(value); k == value)
        addVariable(name, var_type::unsigned_integer, {reinterpret_cast<const std::byte*>(&k), 4});
    else
        addVariable(name, var_type::unsigned_integer, {reinterpret_cast<const std::byte*>(&value), 8});
}

void message_internal::addVariable(std::string_view name, std::span<const std::byte> binary)
{
    addVariable(name, var_type::binary, binary);
}

message_internal::output_buffer message_internal::output() noexcept
{
    return {m_encoded, output_buffer::whence::end, std::endian::little};
}

void message_internal::ensureDecoded() const
{
    if (m_vars.empty())
        const_cast<message_internal*>(this)->decode();
}

void message_internal::finalise() noexcept
{
    // Write the message size.
    auto buffer = output();
    buffer.seek(0, output_buffer::whence::begin);
    buffer.write<uint32le_t>(m_encoded.size());

    m_preEncoded = m_encoded;
}

bool message_internal::decode()
{
    if (!m_vars.empty())
        return true;

    input_buffer input(m_preEncoded, input_buffer::whence::begin, std::endian::little);
    if (input.size() < 4)
        return false;

    auto len = input.read<uint32le_t>();
    if (len > input.size())
        return false;

    auto encoding = readVarInt(input);
    if (!encoding || *encoding > static_cast<std::uint32_t>(encoding_format::current))
        return false;

    while (input.remaining() > 0)
    {
        auto var_len = readVarInt(input);
        if (!var_len || *var_len > input.remaining())
            return false;

        auto header_len = input.remaining();
        auto type = readVarInt(input);
        auto name_len = readVarInt(input);
        if (!type || !name_len)
            return false;
        header_len = header_len - input.remaining();

        if (*name_len > input.remaining() || (*name_len + header_len) > *var_len)
            return false;

        std::string_view name {input.ptr<const char>(), static_cast<std::size_t>(*name_len)};
        input.seek(static_cast<std::ptrdiff_t>(*name_len));
        std::span value {input.ptr<std::byte>(), static_cast<std::size_t>(*var_len - *name_len - header_len)};
        input.seek(value.size());

        m_vars.emplace_back(name, static_cast<var_type>(*type), value);

        if (name == SystemVariable::Message)
            m_msg = {reinterpret_cast<const char*>(value.data()), value.size()};
    }

    return true;
}


std::size_t message_internal::encodedMessageSize(std::span<const std::byte> data)
{
    input_buffer buffer(data, std::endian::little);
    if (buffer.remaining() < 4)
        return 0;

    return buffer.read<uint32le_t>();
}

void message_internal::writeVarInt(output_buffer& buffer, std::uint64_t val)
{
    constexpr std::uint64_t mask = 0x7f;
    while ((val & ~mask) != 0)
    {
        buffer.write<std::uint8_t>(0x80 | (val & mask));
        val >>= 7;
    }
    buffer.write<std::uint8_t>(val);
}

std::optional<std::uint64_t> message_internal::readVarInt(input_buffer& buffer)
{
    std::uint64_t val = 0;
    int shift = 0;
    bool more = true;
    do
    {
        if (buffer.remaining() < 1)
            return {};

        auto i = buffer.read<std::uint8_t>();
        more = (i & 0x80);
        val |= ((i & 0x7f) << shift);
        shift += 7;
    }
    while (more);
    
    return val;
}

std::size_t message_internal::varIntLength(std::uint64_t val)
{
    constexpr std::uint64_t mask = 0x7f;
    std::size_t len = 1;
    while ((val & ~mask) != 0)
    {
        ++len;
        val >>= 7;
    }

    return len;
}

priority message_internal::getPriority() const noexcept
{
    auto var = std::find_if(m_vars.begin(), m_vars.end(), [](const auto& v)
    {
        return v.name == SystemVariable::Priority;
    });

    // Shouldn't happen but we need to return something.
    if (var == m_vars.end())
        return priority::info;

    //! @TODO: fix this (endian, etc).
    std::int32_t prio;
    if (var->data.size() == 1)
        prio = *reinterpret_cast<const std::int8_t*>(var->data.data());
    else if (var->data.size() == 2)
        prio = *reinterpret_cast<const std::int16_t*>(var->data.data());
    else
        prio = *reinterpret_cast<const std::int32_t*>(var->data.data());

    return static_cast<priority>(prio);
}


}

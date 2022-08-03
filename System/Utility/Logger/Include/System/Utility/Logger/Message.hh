#ifndef __SYSTEM_UTILITY_LOGGER_MESSAGE_H
#define __SYSTEM_UTILITY_LOGGER_MESSAGE_H


#include <System/Utility/Logger/Private/Config.hh>

#if defined(__XVI_NO_STDLIB)
#  include <System/C++/Containers/Vector.hh>
#  include <System/C++/LanguageSupport/Compare.hh>
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/Utility/Span.hh>
#  include <System/C++/Utility/StdExcept.hh>
#  include <System/C++/Utility/String.hh>
#  include <System/C++/Utility/StringView.hh>
#else
#  include <compare>
#  include <cstddef>
#  include <span>
#  include <stdexcept>
#  include <string>
#  include <string_view>
#  include <utility>
#  include <vector>
#endif

#include <System/Utility/Logger/Logger.hh>


namespace System::Utility::Logger
{


struct opaque_message_t;
using opaque_message = const opaque_message_t*;


// Message variables with system-defined semantics.
namespace SystemVariable
{

inline constexpr std::string_view Message       = "System.Message";
inline constexpr std::string_view Facility      = "System.Facility";
inline constexpr std::string_view Priority      = "System.Priority";

} // namespace SystemVariable


__SYSTEM_UTILITY_LOGGER_EXPORT void             freeMessage(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.Free);
__SYSTEM_UTILITY_LOGGER_EXPORT opaque_message   copyMessage(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.Copy);

__SYSTEM_UTILITY_LOGGER_EXPORT std::pair<const char*, std::size_t>      getMessageString(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetString);
__SYSTEM_UTILITY_LOGGER_EXPORT priority                                 getMessagePriority(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetPriority);
__SYSTEM_UTILITY_LOGGER_EXPORT std::size_t                              getMessageVarCount(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetVarCount);
__SYSTEM_UTILITY_LOGGER_EXPORT std::pair<const char*, std::size_t>      getMessageVarName(opaque_message, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetVarName);
__SYSTEM_UTILITY_LOGGER_EXPORT var_type                                 getMessageVarType(opaque_message, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetVarType);
__SYSTEM_UTILITY_LOGGER_EXPORT std::pair<const std::byte*, std::size_t> getMessageVarValue(opaque_message, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetVarValue);
__SYSTEM_UTILITY_LOGGER_EXPORT std::pair<const std::byte*, std::size_t> getMessageVarValueByName(opaque_message, const char*, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetVarValueByName);

__SYSTEM_UTILITY_LOGGER_EXPORT std::size_t      getMessageEncodedSize(opaque_message) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.GetEncodedSize);
__SYSTEM_UTILITY_LOGGER_EXPORT std::size_t      encodeMessage(opaque_message, std::byte*, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.Encode);
__SYSTEM_UTILITY_LOGGER_EXPORT opaque_message   decodeMessage(const std::byte*, std::size_t) __SYSTEM_UTILITY_LOGGER_SYMBOL(Message.Decode);


class message_var_proxy
{
public:

    constexpr message_var_proxy() noexcept = default;

    std::string_view name() const noexcept
    {
        auto [ptr, size] = getMessageVarName(m_msg, m_index);
        return {ptr, size};
    }

    var_type type() const noexcept
    {
        return getMessageVarType(m_msg, m_index);
    }

    std::span<const std::byte> data() const noexcept
    {
        auto [ptr, size] = getMessageVarValue(m_msg, m_index);
        return {ptr, size};
    }

private:

    friend class message_var_iterator;
    friend class message;

    opaque_message  m_msg       = nullptr;
    std::size_t     m_index     = 0;

    constexpr message_var_proxy(opaque_message msg, std::size_t index) :
        m_msg(msg),
        m_index(index)
    {
    }
};

class message_var_iterator
{
public:

    using value_type        = message_var_proxy;
    using difference_type   = std::ptrdiff_t;
    using pointer           = message_var_proxy*;
    using reference         = message_var_proxy;
    using iterator_category = std::random_access_iterator_tag;

    constexpr message_var_iterator() noexcept = default;

    constexpr message_var_proxy operator*() const noexcept
    {
        return {m_msg, m_index};
    }

    constexpr bool operator==(const message_var_iterator& x) const = default;
    constexpr std::strong_ordering operator<=>(const message_var_iterator& x) const = default;

    constexpr message_var_iterator& operator++() noexcept
    {
        ++m_index;
        return *this;
    }

    constexpr message_var_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr message_var_iterator& operator--() noexcept
    {
        --m_index;
        return *this;
    }

    constexpr message_var_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr message_var_iterator& operator+=(difference_type n) noexcept
    {
        m_index += static_cast<std::size_t>(n);
        return *this;
    }

    constexpr message_var_iterator& operator-=(difference_type n) noexcept
    {
        return operator+=(-n);
    }

    constexpr message_var_iterator operator+(difference_type n) const noexcept
    {
        auto tmp = *this;
        tmp += n;
        return tmp;
    }

    friend constexpr message_var_iterator operator+(difference_type n, const message_var_iterator& x) noexcept
    {
        return (x + n);
    }

    constexpr message_var_iterator operator-(difference_type n) const noexcept
    {
        return operator+(-n);
    }

    constexpr difference_type operator-(const message_var_iterator& x) const noexcept
    {
        return static_cast<difference_type>(m_index - x.m_index);
    }

    constexpr auto operator[](difference_type n) const noexcept
    {
        auto tmp = *this;
        tmp += n;
        return *tmp;
    }

private:

    friend class message;

    opaque_message  m_msg       = nullptr;
    std::size_t     m_index     = 0;

    constexpr message_var_iterator(opaque_message msg, std::size_t index) noexcept :
        m_msg(msg),
        m_index(index)
    {
    }
};

class message
{
public:

    constexpr message() noexcept = default;

    message(const message& m) noexcept :
        m_msg(copyMessage(m.m_msg))
    {
    }

    message(message&& m) noexcept :
        m_msg(m.m_msg)
    {
        m.m_msg = nullptr;
    }

    message(opaque_message msg) :
        m_msg(msg)
    {
    }

    message& operator=(const message& m) noexcept
    {
        if (&m == this) [[unlikely]]
            return *this;

        m_msg = copyMessage(m.m_msg);
        return *this;
    }

    message& operator=(message&& m) noexcept
    {
        using std::swap;
        swap(m_msg, m.m_msg);
        return *this;
    }

    message& operator=(opaque_message msg) noexcept
    {
        return operator=(message(msg));
    }

    ~message()
    {
        freeMessage(m_msg);
    }

    std::string_view msg() const noexcept
    {
        auto [ptr, size] = getMessageString(m_msg);
        return {ptr, size};
    }

    Logger::priority priority() const noexcept
    {
        return getMessagePriority(m_msg);
    }

    std::size_t encodedSize() const noexcept
    {
        return getMessageEncodedSize(m_msg);
    }

    std::size_t encodeTo(std::span<std::byte> to) const
    {
        return encodeMessage(m_msg, to.data(), to.size());
    }

    std::vector<std::byte> encode() const
    {
        std::vector<std::byte> vec;

        auto size = getMessageEncodedSize(m_msg);
        vec.resize(size);

        encodeMessage(m_msg, vec.data(), vec.size());
        return vec;
    }

    constexpr bool valid() const noexcept
    {
        return m_msg != nullptr;
    }

    constexpr operator bool() const noexcept
    {
        return valid();
    }

    std::size_t size() const noexcept
    {
        return getMessageVarCount(m_msg);
    }

    message_var_proxy operator[](std::size_t n) const
    {
        return {m_msg, n};
    }

    message_var_proxy at(std::size_t n) const
    {
        if (n >= size())
            throw std::out_of_range("invalid index");

        return {m_msg, n};
    }

    message_var_iterator begin() const noexcept
    {
        return {m_msg, 0};
    }

    message_var_iterator end() const noexcept
    {
        if (!m_msg)
            return {nullptr, 0};

        return {m_msg, getMessageVarCount(m_msg)};
    }

    message_var_iterator cbegin() const noexcept
    {
        return begin();
    }

    message_var_iterator cend() const noexcept
    {
        return end();
    }

    opaque_message release() noexcept
    {
        auto tmp = m_msg;
        m_msg = nullptr;
        return tmp;
    }

private:

    opaque_message  m_msg       = nullptr;
};


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_MESSAGE_H */

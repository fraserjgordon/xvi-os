#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODEDSTRING_H
#define __SYSTEM_TEXT_ENCODING_ENCODEDSTRING_H


#include <System/Text/Encoding/Private/Config.hh>

#include <System/Text/Encoding/Codec.hh>
#include <System/Text/Encoding/Iterator.hh>

#include <cstddef>
#include <span>


namespace System::Text::Encoding
{


class EncodedStringView
{
public:

    using iterator      = Iterator;

    Iterator begin() const
    {
        return Iterator{m_codec, m_bytes};
    }

    Iterator end() const
    {
        return Iterator{m_codec, m_bytes.subspan(m_bytes.size(), 0)};
    }

private:

    const Codec*                m_codec = nullptr;
    std::span<const std::byte>  m_bytes = {};
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODEDSTRING_H */

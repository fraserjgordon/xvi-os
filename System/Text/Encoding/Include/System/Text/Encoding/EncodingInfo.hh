#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODINGINFO_H
#define __SYSTEM_TEXT_ENCODING_ENCODINGINFO_H


#include <System/Text/Encoding/EncodingFactory.hh>
#include <System/Text/Encoding/EncodingID.hh>

#include <initializer_list>


namespace System::Text::Encoding
{


struct EncodingInfo
{
    EncodingID                          id      = EncodingID::Unknown;
    std::initializer_list<const char*>  names   = {};
    const EncodingFactory*              factory = nullptr;
};


} // namespace System::TExt::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODINGINFO_H */

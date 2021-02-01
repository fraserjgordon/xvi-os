#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_INFOTABLE_H
#define __SYSTEM_TEXT_ENCODING_INFOTABLE_H


#include <System/Text/Encoding/Private/Config.hh>

#include <System/Text/Encoding/EncodingInfo.hh>

#include <string_view>

namespace System::Text::Encoding
{


__SYSTEM_TEXT_ENCODING_EXPORT const EncodingInfo* getEncodingById(EncodingID) __SYSTEM_TEXT_ENCODING_SYMBOL(System.Text.Encoding.GetEncodingById);
__SYSTEM_TEXT_ENCODING_EXPORT const EncodingInfo* getEncodingByName(std::string_view) __SYSTEM_TEXT_ENCODING_SYMBOL(System.Text.Encoding.GetEncodingByName);


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_INFOTABLE_H */

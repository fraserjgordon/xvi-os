#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODERIMPL_H
#define __SYSTEM_TEXT_ENCODING_ENCODERIMPL_H


#include <System/Text/Encoding/Encoder.hh>


namespace System::Text::Encoding
{


class EncoderImpl :
    public Encoder
{

};


template <class CodecType>
class StatelessEncoder final :
    public EncoderImpl
{
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODERIMPL_H */

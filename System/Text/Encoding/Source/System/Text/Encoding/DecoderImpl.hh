#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_DECODERIMPL_H
#define __SYSTEM_TEXT_ENCODING_DECODERIMPL_H


#include <System/Text/Encoding/Decoder.hh>


namespace System::Text::Encoding
{


class DecoderImpl :
    public Decoder
{

};


template <class CodecType>
class StatelessDecoder final :
    public DecoderImpl
{
};



} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_DECODERIMPL_H */

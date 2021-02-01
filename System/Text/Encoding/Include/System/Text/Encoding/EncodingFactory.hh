#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODINGFACTORY_H
#define __SYSTEM_TEXT_ENCODING_ENCODINGFACTORY_H


#include <System/Text/Encoding/Private/Config.hh>

#include <memory>


namespace System::Text::Encoding
{


// Forward declarations.
class Codec;    //!< Stateless encode/decode functions.
class Encoder;  //!< Stateful encoder.
class Decoder;  //!< Stateful decoder.


class EncodingFactory
{
public:

    const Codec& getCodec() const noexcept
    {
        return *encodingFactoryGetCodec(this);
    }

    std::unique_ptr<Encoder> createEncoder() const
    {
        return std::unique_ptr<Encoder>{encodingFactoryCreateEncoder(this)};
    }

    std::unique_ptr<Decoder> createDecoder() const
    {
        return std::unique_ptr<Decoder>{encodingFactoryCreateDecoder(this)};
    }

private:

    __SYSTEM_TEXT_ENCODING_EXPORT static const Codec*   encodingFactoryGetCodec(const EncodingFactory*) __SYSTEM_TEXT_ENCODING_SYMBOL(EncodingFactory.GetCodec);
    __SYSTEM_TEXT_ENCODING_EXPORT static Encoder*       encodingFactoryCreateEncoder(const EncodingFactory*) __SYSTEM_TEXT_ENCODING_SYMBOL(EncodingFactory.CreateEncoder);
    __SYSTEM_TEXT_ENCODING_EXPORT static Decoder*       encodingFactoryCreateDecoder(const EncodingFactory*) __SYSTEM_TEXT_ENCODING_SYMBOL(EncodingFactory.CreateDecoder);
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODINGFACTORY_H */

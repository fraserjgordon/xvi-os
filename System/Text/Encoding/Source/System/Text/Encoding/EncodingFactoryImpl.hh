#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODINGFACTORYIMPL_H
#define __SYSTEM_TEXT_ENCODING_ENCODINGFACTORYIMPL_H


#include <System/Text/Encoding/CodecImpl.hh>
#include <System/Text/Encoding/DecoderImpl.hh>
#include <System/Text/Encoding/EncoderImpl.hh>
#include <System/Text/Encoding/EncodingFactory.hh>


namespace System::Text::Encoding
{


class EncodingFactoryImplBase :
    public EncodingFactory
{
public:

    // Note: these shadow the non-virtual declarations in the base class deliberately.
    virtual const Codec& getCodec() const = 0;
    virtual Encoder* createEncoder() const = 0;
    virtual Decoder* createDecoder() const = 0;

    static const EncodingFactoryImplBase* fromEncodingFactory(const EncodingFactory* ef)
    {
        return static_cast<const EncodingFactoryImplBase*>(ef);
    }
};

template <class CodecType, class EncoderType = StatelessEncoder<CodecType>, class DecoderType = StatelessDecoder<CodecType>>
class EncodingFactoryImpl final :
    public EncodingFactoryImplBase
{
public:

    const Codec& getCodec() const final
    {
        return reinterpret_cast<const Codec&>(s_codec);
    }

    Encoder* createEncoder() const final
    {
        return new EncoderType{};
    }

    Decoder* createDecoder() const final
    {
        return new DecoderType{};
    }


    static constexpr const EncodingFactory& instance()
    {
        return s_instance;
    }

private:

    inline static const CodecType s_codec = {};
    inline static constexpr EncodingFactoryImpl s_instance = {};
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODINGFACTORYIMPL_H */

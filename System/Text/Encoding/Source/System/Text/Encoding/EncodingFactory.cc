#include <System/Text/Encoding/EncodingFactoryImpl.hh>


namespace System::Text::Encoding
{


const Codec* EncodingFactory::encodingFactoryGetCodec(const EncodingFactory* ef)
{
    return &EncodingFactoryImplBase::fromEncodingFactory(ef)->getCodec();
}

Encoder* EncodingFactory::encodingFactoryCreateEncoder(const EncodingFactory* ef)
{
    return EncodingFactoryImplBase::fromEncodingFactory(ef)->createEncoder();
}

Decoder* EncodingFactory::encodingFactoryCreateDecoder(const EncodingFactory* ef)
{
    return EncodingFactoryImplBase::fromEncodingFactory(ef)->createDecoder();
}


} // namespace System::Text::Encoding

#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ITERATORIMPL_H
#define __SYSTEM_TEXT_ENCODING_ITERATORIMPL_H


#include <System/Text/Encoding/EncodingFactoryImpl.hh>
#include <System/Text/Encoding/Iterator.hh>


namespace System::Text::Encoding
{


struct iterator_ops_t
{
    void (*increment)(const std::byte*& ptr, Iterator::context_t& context);
    char32_t (*read)(const std::byte* ptr, Iterator::context_t& context);
};


class IteratorImpl :
    public Iterator
{
public:

    void increment()
    {
        ops()->increment(m_ptr, m_context);
    }

    char32_t read() const
    {
        return ops()->read(m_ptr, const_cast<context_t&>(m_context));
    }


    static IteratorImpl* fromIterator(Iterator* i)
    {
        return static_cast<IteratorImpl*>(i);
    }

    static const IteratorImpl* fromIterator(const Iterator* i)
    {
        return static_cast<const IteratorImpl*>(i);
    }

private:

    const iterator_ops_t* ops() const
    {
        return static_cast<const iterator_ops_t*>(m_vtbl);
    }
};


template <class CodecType>
class StatelessIterator :
    public IteratorImpl
{
private:

    using FactoryType = EncodingFactoryImpl<CodecType>;

    static void increment(const std::byte*& ptr, Iterator::context_t&)
    {
        ptr = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(ptr) + CodecType::CodeUnitSize);
    }

    static char32_t read(const std::byte* ptr, Iterator::context_t&)
    {
        return FactoryType::instance().getCodec().decodeCodepoint({ptr, CodecType::CodeUnitSize});
    } 


    static constexpr const iterator_ops_t s_ops =
    {
        .increment = &increment,
        .read = &read
    };
};


}


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ITERATORIMPL_H */

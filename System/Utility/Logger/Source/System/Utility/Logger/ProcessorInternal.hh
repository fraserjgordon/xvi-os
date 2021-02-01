#ifndef __SYSTEM_UTILITY_LOGGER_PROCESSORINTERNAL_H
#define __SYSTEM_UTILITY_LOGGER_PROCESSORINTERNAL_H


#include <System/Utility/Logger/Private/Config.hh>

#include <array>
#include <cstddef>
#include <span>

#include <System/Utility/CppExtra/RefcountedPtr.hh>

#include <System/Utility/Logger/CircularBuffer.hh>
#include <System/Utility/Logger/Processor.hh>


namespace System::Utility::Logger
{


class processor_base :
    public CppExtra::refcounted_mixin
{
public:

    using pointer = CppExtra::refcounted_ptr<processor_base>;

    virtual void processMessage(message_internal::pointer);
    virtual void processEncodedMessage(std::span<const std::byte>);
    virtual std::size_t read(std::ptrdiff_t offset, std::span<std::byte>, bool remove);

    opaque_processor opaque() noexcept
    {
        return reinterpret_cast<opaque_processor>(this);
    }

    
    static pointer fromOpaque(opaque_processor p)
    {
        return pointer(reinterpret_cast<processor_base*>(p));
    }

    static bool addProcessor(pointer);
    static void removeProcessor(pointer);

    static void handleMessage(message_internal::pointer);

protected:

    virtual ~processor_base() = 0;

private:

    friend class CppExtra::refcounted_ptr_traits<processor_base>;

    static constexpr std::size_t MaxProcessors  = 8;

    static std::array<pointer, MaxProcessors> s_processors;
};


class message_processor final :
    public processor_base
{
public:

    constexpr message_processor(message_processor_callback cb, void* ctxt) :
        m_callback(cb),
        m_callbackContext(ctxt)
    {
    }

    void processMessage(message_internal::pointer message) override
    {
        m_callback(m_callbackContext, message->opaque());
    }

private:

    message_processor_callback  m_callback          = nullptr;
    void*                       m_callbackContext   = nullptr;

    ~message_processor() final = default;
};

class encoded_message_processor final :
    public processor_base
{
public:

    constexpr encoded_message_processor(encoded_processor_callback cb, void* ctxt) :
        m_callback(cb),
        m_callbackContext(ctxt)
    {
    }

    void processEncodedMessage(std::span<const std::byte> data) override
    {
        m_callback(m_callbackContext, data.data(), data.size());
    }

private:

    encoded_processor_callback  m_callback          = nullptr;
    void*                       m_callbackContext   = nullptr;

    ~encoded_message_processor() final = default;
};

class buffer_message_processor final :
    public processor_base
{
public:

    buffer_message_processor(buffering_params params) :
        m_buffer({params.buffer_ptr, params.buffer_size})
    {
    }

    void processEncodedMessage(std::span<const std::byte>) override;

    std::size_t read(std::ptrdiff_t, std::span<std::byte>, bool) override;

private:

    circular_buffer             m_buffer;

    ~buffer_message_processor() final = default;
};


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_PROCESSORINTERNAL_H */

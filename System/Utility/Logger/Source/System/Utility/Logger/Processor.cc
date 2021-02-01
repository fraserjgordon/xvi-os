#include <System/Utility/Logger/ProcessorInternal.hh>


namespace System::Utility::Logger
{


std::array<processor_base::pointer, processor_base::MaxProcessors> processor_base::s_processors = {};


opaque_processor registerMessageProcessor(message_processor_callback cb, void* ctxt)
{
    processor_base::pointer p(new message_processor(cb, ctxt));
    if (!processor_base::addProcessor(p))
        return nullptr;

    return p.take()->opaque();
}

opaque_processor registerEncodedProcessor(encoded_processor_callback cb, void* ctxt)
{
    processor_base::pointer p(new encoded_message_processor(cb, ctxt));
    if (!processor_base::addProcessor(p))
        return nullptr;

    return p.take()->opaque();
}

opaque_processor registerBufferProcessor(const buffering_params* params)
{
    processor_base::pointer p(new buffer_message_processor(*params));
    if (!processor_base::addProcessor(p))
        return nullptr;

    return p.take()->opaque();
}

void unregisterProcessor(opaque_processor op)
{
    auto p = processor_base::fromOpaque(op);
    processor_base::removeProcessor(p);
}

std::size_t readFromBufferProcessor(opaque_processor op, std::ptrdiff_t offset, std::byte* buffer, std::size_t size, bool remove)
{
    auto p = processor_base::fromOpaque(op).take();
    return p->read(offset, {buffer, size}, remove);
}


void processor_base::processMessage(message_internal::pointer)
{
}

void processor_base::processEncodedMessage(std::span<const std::byte>)
{
}

std::size_t processor_base::read(std::ptrdiff_t, std::span<std::byte>, bool)
{
    return 0;
}

bool processor_base::addProcessor(pointer p)
{
    if (!p)
        return false;

    for (auto& entry : s_processors)
    {
        if (!entry)
        {
            entry = p;
            return true;
        }
    }

    return false;
}

void processor_base::removeProcessor(pointer p)
{
    for (auto& entry : s_processors)
    {
        if (entry == p)
        {
            entry = nullptr;
            break;
        }
    }
}

void processor_base::handleMessage(message_internal::pointer msg)
{
    for (const auto& p : s_processors)
    {
        if (p)
        {
            p->processMessage(msg);
            p->processEncodedMessage(msg->data());
        }
    }
}

processor_base::~processor_base()
{
}


void buffer_message_processor::processEncodedMessage(std::span<const std::byte> data)
{
    m_buffer.add(data);
}

std::size_t buffer_message_processor::read(std::ptrdiff_t offset, std::span<std::byte> data, bool remove)
{
    // Attempt to read as many messages as possible.
    std::size_t size_read = 0;
    while (true)
    {
        auto msg_size = m_buffer.extractTo(offset, data, remove);
        if (msg_size > data.size() || msg_size == 0)
        {
            if (size_read == 0)
                return msg_size;
            else
                break;
        }

        size_read += msg_size;
        data = data.subspan(msg_size);
    }

    return size_read;
}


} // namespace System::Utility::Logger

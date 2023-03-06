#ifndef __SYSTEM_IO_FILEIO_IOHANDLE_H
#define __SYSTEM_IO_FILEIO_IOHANDLE_H


#include <System/IO/FileIO/Handle.hh>


namespace System::IO::FileIO
{


class IOHandle :
    public Handle
{
public:

    constexpr IOHandle() noexcept = default;
    IOHandle(const IOHandle&) = delete;
    constexpr IOHandle(IOHandle&&) noexcept = default;

    constexpr IOHandle(AbstractHandle h) :
        Handle(h)
    {
    }

    ~IOHandle() override = default;

    void operator=(const IOHandle&) = delete;
    IOHandle& operator=(IOHandle&&) noexcept = default;


    template <std::derived_from<IOHandle> T> IOHandle(T&&) = delete;
    template <std::derived_from<IOHandle> T> void operator=(T&&) = delete;


    result<IOHandle> clone() const noexcept;

    virtual result<std::span<buffer>> read(offset_t, std::span<buffer>) noexcept;

    virtual result<std::span<const_buffer>> write(offset_t, std::span<const_buffer>) noexcept;
    result<std::span<buffer>> write(offset_t, std::span<buffer>) noexcept;

    virtual std::size_t maxBufferCount() const noexcept;

    virtual bool isSized() const noexcept
    {
        return isSeekable();
    }

    virtual result<offset_t> size() const noexcept;
};

using SharedIOHandle = SharedHandle<IOHandle>;
using UniqueIOHandle = UniqueHandle<IOHandle>;


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_IOHANDLE_H */

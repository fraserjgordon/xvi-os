#include <System/Storage/VirtualDisk/RawVirtualDisk.hh>

#include <cstring>


namespace System::Storage::VirtualDisk
{


namespace FIO = System::IO::FileIO;


RawVirtualDisk::RawVirtualDisk(std::unique_ptr<FIO::IOHandle> io, const disk_params_t& params) noexcept :
    m_io{std::move(io)},
    m_params{params}
{
    m_length = m_io->size().value_or(0) / m_params.sector_size;
}


std::uint32_t RawVirtualDisk::sectorSize() const noexcept
{
    return m_params.sector_size;
}


RawVirtualDisk::lba_t RawVirtualDisk::sectorCount() const noexcept
{
    return m_length;
}


RawVirtualDisk::result<void> RawVirtualDisk::read(lba_t lba, buffer output) const noexcept
{
    // Partial reads are possible so we need to loop until done.
    auto remaining = output.size();
    auto target = output;
    auto offset = static_cast<FIO::offset_t>(lba * m_params.sector_size);

    while (remaining > 0)
    {
        // Try to read directly into the output buffer.
        auto sgbuffer = FIO::buffer(target.data(), target.size());
        auto res = m_io->read(offset, {&sgbuffer, 1});
        if (!res)
        {
            // Was the error something we should ignore?
            auto error = res.error();
            if (error == std::errc::interrupted)
                continue; // Syscall interrupted by a signal. Try again.

            return std::unexpected(error);
        }

        // Check for zero-length reads. Because we would never ask for a zero-length read, this shouldn't happen.
        if (res->size() == 0)
            return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));

        // Did we read into the output buffer or were we handed a different one?
        auto count = res->data()->size();
        if (const auto& where = *res->data(); where.data() != target.data())
        {
            // We were handed a different buffer. Fulfil this method's contract by copying it into place.
            std::memcpy(target.data(), where.data(), where.size());
        }

        // Update the remaining byte counter.
        remaining -= count;
        target = target.subspan(count);
    }

    return {};
}


RawVirtualDisk::result<void> RawVirtualDisk::write(lba_t lba, const_buffer input) noexcept
{
    // Partial writes are possible so we need to loop until done.
    auto remaining = input.size();
    auto target = input;
    auto offset = static_cast<FIO::offset_t>(lba * m_params.sector_size);

    while (remaining > 0)
    {
        // Try to read directly into the output buffer.
        auto sgbuffer = FIO::const_buffer(target.data(), target.size());
        auto res = m_io->write(offset, {&sgbuffer, 1});
        if (!res)
        {
            // Was the error something we should ignore?
            auto error = res.error();
            if (error == std::errc::interrupted)
                continue; // Syscall interrupted by a signal. Try again.

            return std::unexpected(error);
        }

        // Check for zero-length writes. Because we would never ask for a zero-length write, this shouldn't happen.
        if (res->size() == 0)
            return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));

        // Update the remaining byte counter.
        auto count = res->data()->size();
        remaining -= count;
        target = target.subspan(count);
    }

    return {};
}


RawVirtualDisk RawVirtualDisk::createFrom(std::unique_ptr<FIO::IOHandle> io, const disk_params_t& params) noexcept
{
    return RawVirtualDisk(std::move(io), params);
}


} // namespace System::Storage::VirtualDisk

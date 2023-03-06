#ifndef __SYSTEM_STORAGE_FILESYSTEM_FILESYSTEM_H
#define __SYSTEM_STORAGE_FILESYSTEM_FILESYSTEM_H


#include <expected>
#include <memory>
#include <system_error>

#include <System/Storage/BlockDevice/BlockDev.hh>


namespace System::Storage::Filesystem
{


class Directory;


class Filesystem
{
public:

    using BlockDev = System::Storage::BlockDevice::BlockDev;
    using BlockHandle = BlockDev::BlockHandle;

    template <class T> using result_t = std::expected<T, std::error_code>;


    virtual ~Filesystem() = 0;

    virtual BlockDev* blockDevice() noexcept = 0;
    virtual const BlockDev* blockDevice() const noexcept = 0;
};


} // namespace System::Storage::Filesystem


#endif /* ifndef __SYSTEM_STORAGE_FILESYSTEM_FILESYSTEM_H */

#include <System/Filesystem/FAT/File.hh>
#include <System/Filesystem/FAT/FileImpl.hh>


namespace System::Filesystem::FAT
{


bool File::truncate(std::uint32_t length)
{
    return FileImpl::from(this).truncate(length);
}


bool File::append(std::span<const std::byte> data)
{
    return FileImpl::from(this).append(data);
}


bool File::write(std::uint32_t offset, std::span<const std::byte> data)
{
    return FileImpl::from(this).write(offset, data);
}


FileImpl::FileImpl(const ObjectInfoImpl& info) :
    ObjectImpl{info}
{
}


std::unique_ptr<FileImpl> FileImpl::open(const ObjectInfoImpl& info)
{
    // Check that the named object describes a directory.
    if (info.type() != ObjectType::File)
        return nullptr;

    return std::unique_ptr<FileImpl>{static_cast<FileImpl*>(ObjectImpl::open(info).release())};
}


} // namespace System::Filesystem::FAT

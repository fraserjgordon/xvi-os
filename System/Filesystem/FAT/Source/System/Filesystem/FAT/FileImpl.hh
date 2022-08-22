#ifndef __SYSTEM_FILESYSTEM_FAT_FILEIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_FILEIMPL_H


#include <System/Filesystem/FAT/File.hh>

#include <System/Filesystem/FAT/ObjectImpl.hh>


namespace System::Filesystem::FAT
{


// Forward declarations.
class ObjectImpl;
class ObjectInfoImpl;


class FileImpl :
    public ObjectImpl
{
    friend class ObjectImpl;

public:

    // Like ObjectImpl::open but with additional checking to ensure the object is a file.
    static std::unique_ptr<FileImpl> open(const ObjectInfoImpl& info);


    File* asFile()
    {
        return reinterpret_cast<File*>(this);
    }

    static FileImpl& from(File* dir)
    {
        return reinterpret_cast<FileImpl&>(*dir);
    }

    static const FileImpl& from(const File* dir)
    {
        return reinterpret_cast<const FileImpl&>(*dir);
    }

private:

    FileImpl(const ObjectInfoImpl&);
};


};


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILEIMPL_H */

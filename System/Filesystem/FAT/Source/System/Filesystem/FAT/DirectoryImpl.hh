#ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H


#include <System/Filesystem/FAT/Directory.hh>

#include <System/Filesystem/FAT/ObjectImpl.hh>


namespace System::Filesystem::FAT
{


// Forward declarations.
class ObjectImpl;
class ObjectInfoImpl;


class DirectoryImpl :
    public ObjectImpl
{
    friend class ObjectImpl;

public:

    struct lfn_state_t
    {
        std::array<char16_t, 255>   lfn = {};
        std::uint8_t                length = 0;
        std::uint8_t                ordinal = 0;
        std::uint8_t                checksum = 0;

        static constexpr std::size_t CharsPerLFNEntry = 13;
        static constexpr std::uint8_t MinLFNOrdinal = 1;
        static constexpr std::uint8_t MaxLFNOrdinal = 21;
        static constexpr std::size_t MaxLFNNameLength = 255;


        bool applyLastLFNEntry(const lfn_dirent_t&);

        bool applyLFNEntry(const lfn_dirent_t&);

        bool validateShortNameEntry(const dirent_t&);

        std::array<char16_t, 13> extractChars(const lfn_dirent_t&);
    };

    using enumerate_callback_t = Directory::enumerate_callback_t;
    using enumerate_internal_callback_t = std::function<std::pair<bool, bool> (ObjectInfoImpl&)>;


    void enumerate(enumerate_callback_t);
    void enumerateInternal(enumerate_internal_callback_t);

    std::unique_ptr<ObjectInfoImpl> find(std::string_view name);

    bool updateObjectMetadata(const ObjectInfoImpl& info);


    // Like ObjectImpl::open but with additional checking to ensure the object is a directory.
    static std::unique_ptr<DirectoryImpl> open(const ObjectInfoImpl& info);


    Directory* asDirectory()
    {
        return reinterpret_cast<Directory*>(this);
    }

    static DirectoryImpl& from(Directory* dir)
    {
        return reinterpret_cast<DirectoryImpl&>(*dir);
    }

    static const DirectoryImpl& from(const Directory* dir)
    {
        return reinterpret_cast<const DirectoryImpl&>(*dir);
    }

private:

    DirectoryImpl(const ObjectInfoImpl&);

    ObjectInfoImpl createObjectInfo(const dirent_t&, const lfn_state_t&);
};


};


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H */

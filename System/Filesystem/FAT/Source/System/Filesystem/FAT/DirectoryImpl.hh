#ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H


#include <System/Filesystem/FAT/Directory.hh>

#include <System/Filesystem/FAT/ObjectImpl.hh>


namespace System::Filesystem::FAT
{


class DirectoryImpl :
    public ObjectImpl
{
public:

    using enumerate_callback_t = Directory::enumerate_callback_t;

    struct lfn_state_t
    {
        std::array<char16_t, 255>   lfn = {};
        std::uint8_t                length = 0;
        std::uint8_t                ordinal = 0;
        std::uint8_t                checksum = 0;

        static constexpr std::size_t CharsPerLFNEntry = 13;


        bool applyLastLFNEntry(const lfn_dirent_t&);

        bool applyLFNEntry(const lfn_dirent_t&);

        bool validateShortNameEntry(const dirent_t&);
    };


    void enumerate(enumerate_callback_t);


    static DirectoryImpl& from(Directory* dir)
    {
        return reinterpret_cast<DirectoryImpl&>(*dir);
    }

    static const DirectoryImpl& from(const Directory* dir)
    {
        return reinterpret_cast<const DirectoryImpl&>(*dir);
    }

private:

    static ObjectInfoImpl createObjectInfo(const dirent_t&, const lfn_state_t&);
};


};


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORYIMPL_H */

#pragma once
#ifndef __SYSTEM_FS_FAT_UNALIGNED_H
#define __SYSTEM_FS_FAT_UNALIGNED_H

namespace System::Filesystem::FAT
{


// TODO: refactor this into a common utility header.
template <class T>
class unaligned
{
public:

    using underlying_type = typename T::underlying_native_endian_type;
    
    unaligned() = default;
    unaligned(const unaligned&) = default;
    unaligned(unaligned&&) = default;
    unaligned& operator=(const unaligned&) = default;
    unaligned& operator=(unaligned&&) = default;
    ~unaligned() = default;

    constexpr unaligned(underlying_type t) noexcept
    {
        operator=(t);
    }

    constexpr unaligned& operator=(underlying_type t) noexcept
    {
        __builtin_memcpy(m_data, std::addressof(t), sizeof(underlying_type));
        return *this;
    }

    constexpr operator underlying_type() const noexcept
    {
        underlying_type t;
        __builtin_memcpy(std::addressof(t), m_data, sizeof(underlying_type));
        return t;
    }


private:

    std::byte m_data[sizeof(underlying_type)];
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FS_FAT_UNALIGNED_H */

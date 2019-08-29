#pragma once
#ifndef __SYSTEM_FS_FAT_UNALIGNED_H
#define __SYSTEM_FS_FAT_UNALIGNED_H


namespace System::Filesystem::FAT
{


template <class T>
class unaligned
{
    unaligned() = default;
    unaligned(const unaligned&) = default;
    unaligned(unaligned&&) = default;
    unaligned& operator=(const unaligned&) = default;
    unaligned& operator=(unaligned&&) = default;
    ~unaligned() = default;

    constexpr unaligned(T t) noexcept
    {
        operator=(t);
    }

    constexpr unaligned& operator=(T t) noexcept
    {
        __builtin_memcpy(m_data, std::addressof(t), sizeof(T));
    }

    constexpr operator T() const noexcept
    {
        T t;
        __builtin_memcpy(std::addressof(t), m_data, sizeof(T));
        return t;
    }


private:

    std::byte m_data[sizeof(T)];
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FS_FAT_UNALIGNED_H */

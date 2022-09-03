#ifndef __SYSTEM_UTILITY_ENDIAN_UNALIGNED_H
#define __SYSTEM_UTILITY_ENDIAN_UNALIGNED_H


#include <System/Utility/Endian/Endian.hh>


namespace System::Utility::Endian
{


template <class T>
class [[gnu::packed]] unaligned
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
        auto ts = T::bswap(t);
        __builtin_memcpy(m_data, std::addressof(ts), sizeof(ts));
        return *this;
    }

    constexpr underlying_type value() const noexcept
    {
        underlying_type t;
        __builtin_memcpy(std::addressof(t), m_data, sizeof(underlying_type));
        return T::bswap(t);
    }

    constexpr operator underlying_type() const noexcept
    {
        return value();
    }


private:

    std::byte m_data[sizeof(underlying_type)];
};


using uint16ule_t = unaligned<uint16le_t>;
using uint32ule_t = unaligned<uint32le_t>;
using uint64ule_t = unaligned<uint64le_t>;

using int16ule_t = unaligned<int16le_t>;
using int32ule_t = unaligned<int32le_t>;
using int64ule_t = unaligned<int64le_t>;

using uint16ube_t = unaligned<uint16be_t>;
using uint32ube_t = unaligned<uint32be_t>;
using uint64ube_t = unaligned<uint64be_t>;

using int16ube_t = unaligned<int16be_t>;
using int32ube_t = unaligned<int32be_t>;
using int64ube_t = unaligned<int64be_t>;



} // namespace System::Utility::Endian


#endif /* ifndef __SYSTEM_UTILITY_ENDIAN_UNALIGNED_H */

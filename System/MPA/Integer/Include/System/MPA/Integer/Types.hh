#ifndef __SYSTEM_MPA_INTEGER_TYPES_H
#define __SYSTEM_MPA_INTEGER_TYPES_H


#include <cstdint>
#include <type_traits>


namespace System::MPA::Integer
{


//! @todo auto-detect this
inline constexpr bool Use64BitWords = (sizeof(void*) >= 8);


using word_t    = std::conditional_t<Use64BitWords, std::uint64_t, std::uint32_t>;
using count_t   = std::size_t;


} // namespace System::MPA::Integer


#endif /* ifndef __SYSTEM_MPA_INTEGER_TYPES_H */

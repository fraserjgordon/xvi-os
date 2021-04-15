#ifndef __SYSTEM_UNICODE_UTF_ENCODESTATE_HH
#define __SYSTEM_UNICODE_UTF_ENCODESTATE_HH


#include <System/Unicode/UTF/Private/Config.hh>

#ifdef __SYSTEM_UNICODE_UTF_MINIUTF
#  include <System/C++/LanguageSupport/StdInt.hh>
#else
#  include <cstdint>
#endif


namespace System::Unicode::UTF
{


//! @struct encode_state
//! @brief  Opaque structure used to store UTF-8 encoder/decoder state.
//!
//! @note   The size and alignment of this structure form part of the ABI contract for this library but the contents do
//!         not, except for a guarantee that setting the contents to all-zero represents the initial state. The state
//!         is also guaranteed to not hold ownership of resources nor to have a dependence on its address and can
//!         therefore be copied, duplicated, etc without special treatment.
//!
struct encode_state
{
    std::uint8_t    state[8] alignas(std::uintptr_t) = {};
};


__SYSTEM_UNICODE_UTF_EXPORT bool EncodeStateIsInitial(const encode_state*) __SYSTEM_UNICODE_UTF_SYMBOL(EncodeState.IsInitial);


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_ENCODESTATE_HH */

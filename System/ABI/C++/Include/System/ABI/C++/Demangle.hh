#ifndef __SYSTEM_ABI_CXX_DEMANGLE_H
#define __SYSTEM_ABI_CXX_DEMANGLE_H


#include <System/ABI/C++/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>


namespace __SYSTEM_ABI_CXX_NS
{


//! @brief  Attempts to demangle the given string as a C++ symbol or type name.
//!
//! @warning    If a non-null @p buf parameter is given, it must have been
//!             allocated using @ref malloc as @c __cxa_demangle may call either
//!             @ref realloc or @ref free on it if it was not big enough for the
//!             demangled name. In this case, the returned pointer will point to
//!             the new buffer instead of being equal to @p buf.
//!
//! @note       It is the caller's responsibility to free any memory that is
//!             allocated as the output buffer. If automatically allocated by
//!             @c __cxa_demangle, it needs to be freed via a call to @ref free.
//!
//! @returns    @c nullptr on failure; otherwise a pointer to the demangled name.
//!
//! @param[in]  mangled_name    The symbol or type name to be demangled.
//!
//! @param[in]  buf             Output buffer for the demangled name. If null, a
//!                             buffer will be allocated and returned. If
//!                             non-null, @p n must also be non-null and point
//!                             to a value giving the buffer size.
//!
//! @param[in,out]  n           The size of the output buffer. Must be non-null
//!                             if @p buf is non-null. The pointed-to value will
//!                             be updated to the appropriate size if a buffer
//!                             (re-)allocation is made.
//!
//! @param[out] status          Optional pointer to a status output value. All
//!                             non-zero status values indicate that the call
//!                             failed but some particular meanings are defined:
//!                                 -  0: success
//!                                 - -1: failed to allocate memory
//!                                 - -2: mangled name doesn't match known
//!                                       mangling patterns
//!                                 - -3: invalid arguments
//!
extern "C" __SYSTEM_ABI_CXX_DEMANGLE_EXPORT char* __cxa_demangle(const char* mangled_name, char* buf, std::size_t* n, int* status);


} // namespace __SYSTEM_ABI_CXX_NS


#endif /* ifndef __SYSTEM_ABI_CXX_DEMANGLE_H */

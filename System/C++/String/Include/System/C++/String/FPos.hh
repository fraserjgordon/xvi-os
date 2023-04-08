#ifndef __SYSTEM_CXX_STRING_FPOS_H
#define __SYSTEM_CXX_STRING_FPOS_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/C++/String/Private/Config.hh>
#include <System/C++/String/MBState.hh>


namespace __XVI_STD_STRING_NS_DECL
{


// Stream-related types.
using streamoff = int64_t;
using streamsize = ptrdiff_t;


// Not defined in this library.
template <class _State> class fpos;


using streampos = fpos<mbstate_t>;
using wstreampos = fpos<mbstate_t>;
using u8streampos = fpos<mbstate_t>;
using u16streampos = fpos<mbstate_t>;
using u32streampos = fpos<mbstate_t>;


} // namespace __XVI_STD_STRING_NS_DECL


#endif /* ifndef __SYSTEM_CXX_STRING_FPOS_H */

#ifndef __SYSTEM_UNICODE_UTF_UTF8_HH
#define __SYSTEM_UNICODE_UTF_UTF8_HH


#include <System/Unicode/UTF/Private/Config.hh>

#ifdef __SYSTEM_UNICODE_UTF_MINIUTF
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#else
#  include <cstddef>
#  include <cstdint>
#endif

#include <System/Unicode/UTF/EncodeState.hh>


namespace System::Unicode::UTF
{



__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8ByteLengthOfCodepoint(char32_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.ByteLengthOfCodepoint);

__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidLeadByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidLeadByte);
__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidContinuationByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidContinuationByte);

__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8ExpectedByteLengthFromLeadByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.ExpectedByteLengthFromLeadByte);

__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidSequenceSingle(const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidSequenceSingle);
__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidSequenceAll(const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidSequenceAll);

__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8Consume(encode_state*, const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.Consume);
__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8Produce(encode_state*, char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.Produce);



} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTF8_HH */

#include <System/MPA/Integer/Types.hh>


namespace System::MPA::Integer::Primitives
{


#ifndef __SYSTEM_MPA_INTEGER_HAS_ARCH_ADD_N
word_t add_n(word_t* dest, const word_t* src, const word_t* addend, count_t n)
{
    word_t carry = 0;
    while (n > 0)
    {
        word_t s = *src;
        word_t a = *addend;

        word_t sum1 = s + carry;
        word_t sum2 = sum1 + a;

        carry = (sum1 < s) | (sum2 < sum1);

        *dest++ = sum2;

        ++src;
        ++addend;
        --n;
    }

    return carry;
}
#endif


} // namespace System::MPA::Integer::Primitives

#include <System/MPA/Integer/Types.hh>


namespace System::MPA::Integer::Primitives
{


#ifndef __SYSTEM_MPA_INTEGER_HAS_ARCH_ADD_MN
word_t add_mn(word_t* dest, const word_t* m, count_t mlen,  const word_t* n, count_t nlen)
{
    word_t carry = 0;
    while (nlen > 0)
    {
        word_t s = *m;
        word_t a = *n;

        word_t sum1 = s + carry;
        word_t sum2 = sum1 + a;

        carry = (sum1 < s) | (sum2 < sum1);

        *dest++ = sum2;

        ++m;
        ++n;
        --nlen;
        --mlen;
    }

    while (carry && mlen > 0) [[unlikely]]
    {
        word_t sum = *m + carry;
        carry = (sum < carry);

        *dest++ = sum;

        ++m;
        --mlen;
    }

    if (dest != m)
    {
        while (mlen > 0)
        {
            *dest++ = *m++;
            --mlen;
        }
    }

    return carry;
}
#endif


} // namespace System::MPA::Integer::Primitives

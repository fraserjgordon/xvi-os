#include <System/MPA/Integer/Types.hh>


namespace System::MPA::Integer::Primitives
{


#ifndef __SYSTEM_MPA_INTEGER_HAS_ARCH_ADD_1
word_t add_1(word_t* dest, const word_t* src, count_t src_len, word_t addend)
{
    word_t sum = *src + addend;
    word_t carry = (sum < addend);
    
    *dest++ = sum;
    --src_len;

    while (carry && src_len > 0) [[unlikely]]
    {
        sum = *src + carry;
        carry = (sum < carry);

        *dest++ = sum;
        ++src;
        --src_len;
    }

    if (src != dest)
    {
        while (src_len > 0)
        {
            *dest++ = *src++;
            --src_len;
        }
    }

    return carry;
}
#endif


} // namespace System::MPA::Integer::Primitives

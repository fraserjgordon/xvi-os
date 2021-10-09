#ifndef __SYSTEM_MATHS_BIGNUM_INTEGER_TYPES_HH
#define __SYSTEM_MATHS_BIGNUM_INTEGER_TYPES_HH


#include <cstdint>


namespace System::Maths::BigNum::Integer
{


using digit_t   = std::uintptr_t;
using length_t  = std::int32_t;


struct integer_rep_t
{
    digit_t*        value;      // Non-negative magnitude.
    std::int32_t    size;       // Sign and number of digits in the value.
    std::int32_t    reserved;   // Number of allocated digits.
};


} // namespace System::Maths::BigNum::Integer


#endif /* ifndef __SYSTEM_MATHS_BIGNUM_INTEGER_TYPES_HH */

#ifndef __SYSTEM_MATHS_BIGNUM_INTEGER_BASEOPS_H
#define __SYSTEM_MATHS_BIGNUM_INTEGER_BASEOPS_H


#include <System/Maths/BigNum/Integer/Types.hh>


namespace System::Maths::BigNum::Integer::BaseOp
{


digit_t add_n(length_t length, digit_t* out, const digit_t* addend1, const digit_t* addend2);

digit_t add_1(length_t length, digit_t* out, const digit_t* addend1, digit_t addend2);

digit_t add(digit_t* out, const digit_t* addend1, length_t length1, const digit_t* addend2, length_t length2);

digit_t add_n_scr(length_t length, digit_t* out, const digit_t* addend1, const digit_t* addend2);

digit_t add_1_scr(length_t length, digit_t* out, const digit_t* addend1, digit_t addend2);

digit_t add_n_conditional_scr(digit_t condition, length_t length, digit_t* out, const digit_t* addend1, const digit_t* addend2);

digit_t sub_n(length_t length, digit_t* out, const digit_t* minuend, const digit_t* subtrahend);

digit_t sub_1(length_t length, digit_t* out, const digit_t* minuend, digit_t subtrahend);

digit_t sub(digit_t* out, const digit_t* minuend, length_t minuend_length, const digit_t* subtrahend, length_t subtrahend_length);

digit_t sub_n_scr(length_t length, digit_t* out, const digit_t* minuend, const digit_t* subtrahend);

digit_t sub_1_scr(length_t length, digit_t* out, const digit_t* minuend, digit_t subtrahend);

digit_t sub_n_conditional_scr(digit_t condition, length_t length, digit_t* out, const digit_t* minuend, const digit_t* subtrahend);

void mul_n(length_t length, digit_t* __restrict out, const digit_t* __restrict factor1, const digit_t* __restrict factor2, digit_t* scratch = nullptr);

digit_t mul_1(length_t length, digit_t* out, const digit_t* factor1, digit_t factor2);

void mul(digit_t* __restrict out, length_t out_length, const digit_t* __restrict factor1, length_t factor1_length, const digit_t* __restrict factor2, length_t factor2_length, digit_t* scratch = nullptr);

void mul_square(length_t length, digit_t* __restrict out, const digit_t* factor, digit_t* scratch = nullptr);

digit_t mul_add_1(length_t length, digit_t* accumulator, const digit_t* factor1, digit_t factor2);

digit_t mul_sub_1(length_t length, digit_t* accumulator, const digit_t* factor1, digit_t factor2);

void mul_scr(digit_t* __restrict out, length_t out_length, const digit_t* __restrict factor1, length_t factor1_length, const digit_t* __restrict factor2, length_t factor2_length, digit_t* scratch);

void mul_square_scr(length_t length, digit_t* __restrict out, const digit_t* __restrict factor, digit_t* scratch);

// Things to definitely add:
//
// lshift{,_scr}
// rshift{,_scr}
// {and,or,xor,nand,nor,xnor,andn}{,_scr}
// not{,_scr}
// cmp{,_scr}
// is_zero{,_scr}
// set_zero
// popcount
// bitscan_{0,1}_{up,down}
// swap_conditional_scr
//
// Operations needing more thought:
//
//  div/mod
//  GCD
//  power_modulo_scr
//  modular_reduction_scr


} // namespace System::Maths::BigNum::Integer::BaseOp


#endif /* ifndef __SYSTEM_MATHS_BIGNUM_INTEGER_BASEOPS_H */

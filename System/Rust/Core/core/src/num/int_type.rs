macro_rules! int_type
{
    ($T:ty, $Bits:expr, $UnsignedT:ty) => (int_type!{$T, $Bits, $UnsignedT, stringify!{$T}});

    ($t:ty, $bits:expr, $unsigned:ty, $name:expr) =>
    (
        #[lang = $name]
        impl $t
        {
            const _UNSIGNED: bool = !(0 as $t) < 0;
            
            pub const BITS: u32 = $bits;
            pub const MIN: $t = if (Self::_UNSIGNED) { 0 } else { ((1 as $t) << (Self::BITS - 1)) };
            pub const MAX: $t = !Self::MIN;

            /*pub fn from_str_radix(src: &str, radix: u32) -> Result<$t, ParseIntError>;

            pub const fn count_ones(self) -> u32;

            pub const fn count_zeros(self) -> u32;

            pub const fn leading_zeros(self) -> u32;

            pub const fn trailing_zeros(self) -> u32;

            pub const fn leading_ones(self) -> u32;

            pub const fn trailing_ones(self) -> u32;

            pub const fn rotate_left(self, n: u32) -> $t;

            pub const fn rotate_right(self, n: u32) -> $t;

            pub const fn swap_bytes(self) -> $t;

            pub const fn reverse_bits(self) -> $t;

            pub const fn from_be(x: $t) -> $t;

            pub const fn from_le(x: $t) -> $t;

            pub const fn to_be(self) -> $t;

            pub const fn to_le(self) -> $t;

            #[must_use]
            pub const fn checked_add(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_sub(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_mul(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_div(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_div_euclid(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_rem(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_rem_euclid(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_neg(self, rhs: $t) -> Option<$t>;

            #[must_use]
            pub const fn checked_shl(self, rhs: u32) -> Option<$t>;

            #[must_use]
            pub const fn checked_shr(self, rhs: u32) -> Option<$t>;

            #[must_use]
            pub const fn checked_abs(self) -> Option<$t>;

            #[must_use]
            pub const fn checked_pow(self, exp: u32) -> Option<$t>;

            #[must_use]
            pub const fn saturating_add(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn saturating_sub(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn saturating_neg(self) -> $t;

            #[must_use]
            pub const fn saturating_abs(self) -> $t;

            #[must_use]
            pub const fn saturating_mul(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn saturating_pow(self, exp: u32) -> $t;

            #[must_use]
            pub const fn wrapping_add(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_sub(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_mul(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_div(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_div_euclid(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_rem(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_rem_euclid(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_neg(self) -> $t;

            #[must_use]
            pub const fn wrapping_shl(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_shr(self, rhs: $t) -> $t;

            #[must_use]
            pub const fn wrapping_abs(self) -> $t;

            #[must_use]
            pub const fn wrapping_pow(self, exp: u32) -> $t;

            pub fn unsigned_abs(self) -> $unsigned;

            #[must_use]
            pub const fn overflowing_add(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_sub(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_mul(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_div(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_div_euclid(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_rem(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_rem_euclid(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_shl(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_shr(self, rhs: $t) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_abs(self) -> ($t, bool);

            #[must_use]
            pub const fn overflowing_pow(self, exp: u32) -> ($t, bool);

            #[must_use]
            pub fn pow (self, exp: i32) -> $t;

            #[must_use]
            pub fn div_euclid(self, rhs: $t) -> $t;

            #[must_use]
            pub fn rem_euclid(self, rhs: $t) -> $t;

            pub const fn abs(self) -> $t;

            pub const fn signum(self) -> $t;

            pub const fn is_positive(self) -> bool;

            pub const fn is_negative(self) -> bool;

            pub const fn to_bytes(self) -> [u8; Self::BITS / 8];

            pub const fn to_le_bytes(self) -> [u8; Self::BITS / 8];

            pub const fn to_be_bytes(self) -> [u8; Self::BITS / 8];

            pub const fn to_ne_bytes(self) -> [u8; Self::BITS / 8];

            pub const fn from_be_bytes(bytes: [u8; Self::BITS / 8]) -> $t;

            pub const fn from_le_bytes(bytes: [u8; Self::BITS / 8]) -> $t;

            pub const fn from_ne_bytes(bytes: [u8; Self::BITS / 8]) -> $t;
            
            */

            pub const fn min_value() -> $t
            {
                Self::MIN
            }

            pub const fn max_value() -> $t
            {
                Self::MAX
            }
        }
    )
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Bound<T>
{
    Included(T),
    Excluded(T),
    Unbounded,
}

#[lang = "generator_state"]
#[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Eq, Ord, Hash)]
pub enum GeneratorState<Y, R>
{
    Yielded(Y),
    Complete(R),
}

#[lang = "Range"]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Range<Index>
{
    pub start: Index,
    pub end: Index,
}

#[lang = "RangeFrom"]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct RangeFrom<Index>
{
    pub start: Index,
}

#[lang = "RangeFull"]
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub struct RangeFull;

#[lang = "RangeInclusive"]
#[derive(Clone, PartialEq, Eq, Hash)]
pub struct RangeInclusive<Index>
{
    pub(crate) _start: Index,
    pub(crate) _end: Index,

    pub(crate) _finished: bool,
}

#[lang = "RangeTo"]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct RangeTo<Index>
{
    pub end: Index,
}

#[lang = "RangeToInclusive"]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct RangeToInclusive<Index>
{
    pub end: Index,
}


macro_rules! ops_trait
{
    ($Trait:ident, $Id:ident) =>
        (
            ops_trait!{$Trait, $Id, stringify!($Id)}
        );

    ($Trait:ident, $Id:ident, $LangItem:expr) =>
        (
            #[lang = $LangItem]
            pub trait $Trait<Rhs = Self>
            {
                type Output;
                
                #[must_use]
                fn $Id(self, rhs: Rhs) -> Self::Output;
            }
        );
}

macro_rules! ops_assign_trait
{
    ($Trait:ident, $Id:ident) =>
        (
            ops_assign_trait!{$Trait, $Id, stringify!($Id)}
        );

    ($Trait:ident, $Id:ident, $LangItem:expr) =>
        (
            #[lang = $LangItem]
            pub trait $Trait<Rhs = Self>
            {
                fn $Id(&mut self, rhs: Rhs);
            }
        )
}

ops_trait!{Add, add}
ops_trait!{BitAnd, bitand}
ops_trait!{BitOr, bitor}
ops_trait!{BitXor, bitxor}
ops_trait!{Div, div}
ops_trait!{Mul, mul}
ops_trait!{Rem, rem}
ops_trait!{Shl, shl}
ops_trait!{Shr, shr}
ops_trait!{Sub, sub}

ops_assign_trait!{AddAssign, add_assign}
ops_assign_trait!{BitAndAssign, bitand_assign}
ops_assign_trait!{BitOrAssign, bitor_assign}
ops_assign_trait!{BitXorAssign, bitxor_assign}
ops_assign_trait!{DivAssign, div_assign}
ops_assign_trait!{MulAssign, mul_assign}
ops_assign_trait!{RemAssign, rem_assign}
ops_assign_trait!{ShlAssign, shl_assign}
ops_assign_trait!{ShrAssign, shr_assign}
ops_assign_trait!{SubAssign, sub_assign}

#[lang = "neg"]
pub trait Neg
{
    type Output;

    #[must_use]
    fn neg(self) -> Self::Output;
}

#[lang = "not"]
pub trait Not
{
    type Output;

    #[must_use]
    fn not(self) -> Self::Output;
}


#[lang = "deref"]
pub trait Deref
{
    type Target: ?Sized;

    #[must_use]
    fn deref(&self) -> &Self::Target;
}

#[lang = "deref_mut"]
pub trait DerefMut : Deref
{
    fn deref_mut(&mut self) -> &mut Self::Target;
}

#[lang = "drop"]
pub trait Drop
{
    fn drop(&mut self);
}

#[lang = "fn"]
#[must_use]
pub trait Fn<Args> : FnMut<Args>
{
    extern "rust-call" fn call(&self, args: Args) -> Self::Output;
}

#[lang = "fn_mut"]
#[must_use]
pub trait FnMut<Args> : FnOnce<Args>
{
    extern "rust-call" fn call_mut(&mut self, args: Args) -> Self::Output;
}

#[lang = "fn_once"]
#[must_use]
pub trait FnOnce<Args>
{
    #[lang = "fn_once_output"]
    type Output;

    extern "rust-call" fn call_once(self, args: Args) -> Self::Output;
}

#[lang = "generator"]
pub trait Generator<R = ()>
{
    type Yield;
    type Return;

    //fn resume(self: Pin<&mut Self>, arg: R) -> GeneratorState<Self::Yield, Self::Return>;
}

#[lang = "index"]
pub trait Index<Idx: ?Sized>
{
    type Output : ?Sized;

    fn index(&self, index: Idx) -> &Self::Output;
}

#[lang = "index_mut"]
pub trait IndexMut<Idx: ?Sized> : Index<Idx>
{
    fn index_mut(&mut self, index: Idx) -> &mut Self::Output;
}

pub trait RangeBounds<T: ?Sized>
{
    fn start_bound(&self) -> Bound<&T>;

    fn end_bound(&self) -> Bound<&T>;

    #[must_use]
    #[inline]
    fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        T: PartialOrd<U>,
        U: PartialOrd<T>,
    {
        match (self.start_bound(), self.end_bound())
        {
            (Bound::Included(start), Bound::Included(end)) => item.ge(start) && item.le(end),
            (Bound::Included(start), Bound::Excluded(end)) => item.ge(start) && item.lt(end),
            (Bound::Excluded(start), Bound::Excluded(end)) => item.gt(start) && item.lt(end),
            (Bound::Excluded(start), Bound::Included(end)) => item.gt(start) && item.le(end),
            (Bound::Unbounded, Bound::Included(end)) => item.le(end),
            (Bound::Unbounded, Bound::Excluded(end)) => item.lt(end),
            (Bound::Unbounded, Bound::Unbounded) => true,
            (Bound::Included(start), Bound::Unbounded) => item.ge(start),
            (Bound::Excluded(start), Bound::Unbounded) => item.gt(start),
        }
    }
}

#[lang = "receiver"]
pub trait Receiver {}

#[lang = "try"]
pub trait Try
{
    type Ok;
    type Error;

    #[lang = "into_result"]
    fn into_result(self) -> Result<Self::Ok, Self::Error>;

    #[lang = "from_error"]
    fn from_error(error: Self::Error) -> Self;

    #[lang = "from_ok"]
    fn from_ok(ok: Self::Ok) -> Self;
}


#[lang = "coerce_unsized"]
pub trait CoerceUnsized<T: ?Sized> {}

#[lang = "dispatch_from_dyn"]
pub trait DispatchFromDyn<T> {}


impl <T: Clone> Bound<T>
{
    pub fn cloned(self) -> Bound<T>
    {
        match self
        {
            Self::Included(x) => Self::Included(x.clone()),
            Self::Excluded(x) => Self::Excluded(x.clone()),
            Self::Unbounded => Self::Unbounded,
        }
    }
}

impl <Index: PartialOrd<Index>> Range<Index>
{
    #[inline]
    pub fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        Index: PartialOrd<U>,
        U: PartialOrd<Index>,
    {
        item.ge(&self.start) && item.lt(&self.end)
    }

    #[inline]
    pub fn is_empty(&self) -> bool
    {
        self.start.eq(&self.end)
    }
}

impl <Index: PartialOrd<Index>> RangeFrom<Index>
{
    #[inline]
    pub fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        Index: PartialOrd<U>,
        U: PartialOrd<Index>,
    {
        item.ge(&self.start)
    }
}

impl <Index> RangeInclusive<Index>
{
    #[lang = "range_inclusive_new"]
    #[inline]
    pub const fn new(start: Index, end: Index) -> Self
    {
        Self { _start: start, _end: end, _finished: false }
    }

    #[inline]
    pub const fn start(&self) -> &Index
    {
        &self._start
    }

    #[inline]
    pub const fn end(&self) -> &Index
    {
        &self._end
    }

    #[inline]
    pub fn into_inner(self) -> (Index, Index)
    {
        (self._start, self._end)
    }
}

impl <Index: PartialOrd<Index>> RangeInclusive<Index>
{
    #[inline]
    pub fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        Index: PartialOrd<U>,
        U: PartialOrd<Index>,
    {
        item.ge(&self._start) && item.le(&self._end)
    }

    #[inline]
    pub fn is_empty(&self) -> bool
    {
        self._finished || self._start >= self._end
    }
}

impl <Index: PartialOrd<Index>> RangeTo<Index>
{
    #[inline]
    pub fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        Index: PartialOrd<U>,
        U: PartialOrd<Index>,
    {
        item.lt(&self.end)
    }
}

impl <Index: PartialOrd<Index>> RangeToInclusive<Index>
{
    #[inline]
    pub fn contains<U: ?Sized>(&self, item: &U) -> bool
    where
        Index: PartialOrd<U>,
        U: PartialOrd<Index>,
    {
        item.le(&self.end)
    }
}

impl <T: ?Sized> Receiver for &T {}

impl <T: ?Sized> Receiver for &mut T {}


macro_rules! number_op_impl
{
    ($T:ty, $Trait:ident, $Id:ident, $Op:tt) =>
        (
            number_op_impl!{$T, $T, $Trait, $Id, $Op}
        );
    
    ($T:ty, $U:ty, $Trait:ident, $Id:ident, $Op:tt) =>
        (
            impl $Trait<$U> for $T
            {
                type Output = $T;
                
                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(self, rhs: $U) -> Self::Output
                {
                    self $Op rhs
                }
            }

            impl $Trait<$U> for &$T
            {
                type Output = $T;

                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(self, rhs: $U) -> Self::Output
                {
                    *self $Op rhs
                }
            }

            impl<'a, 'b> $Trait<&'a $U> for &'b $T
            {
                type Output = $T;

                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(self, rhs: &'a $U) -> Self::Output
                {
                    *self $Op *rhs
                }
            }

            impl $Trait<&$U> for $T
            {
                type Output = $T;

                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(self, rhs: &$U) -> Self::Output
                {
                    self $Op *rhs
                }
            }
        );
}

macro_rules! number_op_assign_impl
{
    ($T:ty, $Trait:ident, $Id:ident, $Op:tt) =>
        (
            number_op_assign_impl!{$T, $T, $Trait, $Id, $Op}
        );

    ($T:ty, $U:ty, $Trait:ident, $Id:ident, $Op:tt) =>
        (
            impl $Trait<$U> for $T
            {
                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(&mut self, rhs: $U)
                {
                    *self $Op rhs
                }
            }

            impl<'a> $Trait<&'a $U> for $T
            {
                #[inline]
                #[rustc_inherit_overflow_checks]
                fn $Id(&mut self, rhs: &'a $U)
                {
                    *self $Op *rhs
                }
            }
        );
}

macro_rules! shift_ops_impl_type
{
    ($T:ty, $U:ty) =>
        (
            number_op_impl!{$T, $U, Shl, shl, <<}
            number_op_impl!{$T, $U, Shr, shr, >>}

            number_op_assign_impl!{$T, $U, ShlAssign, shl_assign, <<=}
            number_op_assign_impl!{$T, $U, ShrAssign, shr_assign, >>=}
        );
}

macro_rules! shift_ops_impl
{
    ($T:ty) =>
        (
            shift_ops_impl!{$T, i8, i16, i32, i64, i128, u8, u16, u32, u64, u128, isize, usize}
        );

    ($T:ty, $($U:ty),+) =>
        ($(
            shift_ops_impl_type!{$T, $U}
        )+)
}

macro_rules! number_ops_impl
{
    ($($T:ty)*) =>
        ($(
            number_op_impl!{$T, Add, add, +}
            number_op_impl!{$T, Div, div, /}
            number_op_impl!{$T, Mul, mul, *}
            number_op_impl!{$T, Rem, rem, %}
            number_op_impl!{$T, Sub, sub, -}

            number_op_assign_impl!{$T, AddAssign, add_assign, +=}
            number_op_assign_impl!{$T, DivAssign, div_assign, /=}
            number_op_assign_impl!{$T, MulAssign, mul_assign, *=}
            number_op_assign_impl!{$T, RemAssign, rem_assign, %=}
            number_op_assign_impl!{$T, SubAssign, sub_assign, -=}
        )*)
}

macro_rules! int_ops_impl
{
    ($($T:ty)*) =>
        ($(
            number_ops_impl!{$T}

            number_op_impl!{$T, BitAnd, bitand, &}
            number_op_impl!{$T, BitOr, bitor, |}
            number_op_impl!{$T, BitXor, bitxor, ^}

            number_op_assign_impl!{$T, BitAndAssign, bitand_assign, &=}
            number_op_assign_impl!{$T, BitOrAssign, bitor_assign, |=}
            number_op_assign_impl!{$T, BitXorAssign, bitxor_assign, ^=}

            shift_ops_impl!{$T}

            impl Not for $T
            {
                type Output = $T;

                #[inline]
                fn not(self) -> Self::Output
                {
                    !self
                }
            }

            impl Not for &$T
            {
                type Output = $T;

                #[inline]
                fn not(self) -> Self::Output
                {
                    !*self
                }
            }
        )*)
}

macro_rules! signed_int_ops_impl
{
    ($($T:ty)*) =>
        ($(
            int_ops_impl!{$T}

            impl Neg for $T
            {
                type Output = $T;

                #[inline]
                fn neg(self) -> Self::Output
                {
                    -self
                }
            }

            impl Neg for &$T
            {
                type Output = $T;

                #[inline]
                fn neg(self) -> Self::Output
                {
                    -*self
                }
            }
        )*)
}

signed_int_ops_impl!
{
    i8 i16 i32 i64 i128
    isize
}

int_ops_impl!
{
    u8 u16 u32 u64 u128
    usize
}

number_ops_impl!
{
    f32 f64
}


impl Not for bool
{
    type Output = bool;
    
    #[inline]
    fn not(self) -> bool
    {
        !self
    }
}

impl <T: ?Sized> Deref for &T
{
    type Target = T;
    
    fn deref(&self) -> &T
    {
        *self
    } 
}

impl <T: ?Sized> Deref for &mut T
{
    type Target = T;

    fn deref(&self) -> &T
    {
        *self
    }
}

impl <T: ?Sized> !DerefMut for &T {}

impl <T: ?Sized> DerefMut for &mut T
{
    fn deref_mut(&mut self) -> &mut T
    {
        *self
    }
}

use crate::marker::Unsize;

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<&'a U> for &'a T {}

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<&'a mut U> for &'a mut T {}

impl <T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<*const U> for *const T {}

impl <T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<*mut U> for *mut T {}


impl <'a, 'b: 'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<&'a U> for &'b T {}

impl <'a, 'b: 'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<&'a U> for &'b mut T {}

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<&'a mut U> for &'a mut T {}

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*const U> for &'a T {}

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*const U> for &'a mut T {}

impl <'a, T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*mut U> for &'a mut T {}

impl <T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*const U> for *const T {}

impl <T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*const U> for *mut T {}

impl <T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<*mut U> for *mut T {}

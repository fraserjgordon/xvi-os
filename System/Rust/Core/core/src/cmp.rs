#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum Ordering
{
    Less        = -1,
    Equal       = 0,
    Greater     = 1,
}

impl Ordering
{
    #[must_use]
    #[inline]
    pub const fn reverse(self) -> Ordering
    {
        match self
        {
            Ordering::Less => Ordering::Greater,
            Ordering::Equal => Ordering::Equal,
            Ordering::Greater => Ordering::Less,
        }
    }

    #[must_use]
    #[inline]
    pub const fn then(self, other: Ordering) -> Ordering
    {
        match self
        {
            Ordering::Equal => other,
            _ => self
        }
    }

    #[must_use]
    #[inline]
    pub fn then_with<F: FnOnce() -> Ordering>(self, f: F) -> Ordering
    {
        match self
        {
            Ordering::Equal => f(),
            _ => self
        }
    }
}


#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub struct Reverse<T>(pub T);

impl <T: PartialOrd> PartialOrd<Reverse<T>> for Reverse<T>
{
    #[inline]
    fn partial_cmp(&self, other: &Reverse<T>) -> Option<Ordering>
    {
        other.0.partial_cmp(&self.0)
    }

    #[inline]
    fn lt(&self, other: &Reverse<T>) -> bool
    {
        other.0 < self.0
    }

    #[inline]
    fn le(&self, other: &Reverse<T>) -> bool
    {
        other.0 <= self.0
    }

    #[inline]
    fn gt(&self, other: &Reverse<T>) -> bool
    {
        other.0 > self.0
    }

    #[inline]
    fn ge(&self, other: &Reverse<T>) -> bool
    {
        other.0 >= self.0
    }
}

impl <T: Ord> Ord for Reverse<T>
{
    #[inline]
    fn cmp(&self, other: &Reverse<T>) -> Ordering
    {
        other.0.cmp(&self.0)
    }
}


pub trait Eq: PartialEq<Self>
{
    // Required to make rustc happy.
    #[inline]
    fn assert_receiver_is_total_eq(&self) {}
}

pub trait Ord: Eq + PartialOrd<Self>
{
    #[must_use]
    fn cmp(&self, other: &Self) -> Ordering;

    #[must_use]
    fn max(self, other: Self) -> Self
    where
        Self: Sized,
    {
        if self.cmp(&other) == Ordering::Less
        {
            self
        }
        else
        {
            other
        }
    }

    #[must_use]
    fn min(self, other: Self) -> Self
    where
        Self: Sized,
    {
        if self.cmp(&other) == Ordering::Less
        {
            other
        }
        else
        {
            self
        }
    }

    #[must_use]
    fn clamp(self, min: Self, max: Self) -> Self
    where
        Self: Sized
    {
        if min.cmp(&max) == Ordering::Greater { panic!() }

        if self.cmp(&max) == Ordering::Greater
        {
            max
        }
        else if self.cmp(&min) == Ordering::Less
        {
            min
        }
        else
        {
            self
        }
    }
}

#[lang = "eq"]
pub trait PartialEq <Rhs: ?Sized = Self>
{
    #[must_use]
    fn eq(&self, other: &Rhs) -> bool;

    #[must_use]
    fn ne(&self, other: &Rhs) -> bool
    {
        !self.eq(&other)
    }
}

#[lang = "partial_ord"]
pub trait PartialOrd <Rhs: ?Sized = Self> : PartialEq<Self>
{
    #[must_use]
    fn partial_cmp(&self, other: &Rhs) -> Option<Ordering>;

    #[must_use]
    #[inline]
    fn lt(&self, other: &Rhs) -> bool
    {
        self.partial_cmp(&other) == Some(Ordering::Less)
    }

    #[must_use]
    #[inline]
    fn le(&self, other: &Rhs) -> bool
    {
        let res = self.partial_cmp(&other);
        res == Some(Ordering::Less) || res == Some(Ordering::Equal)
    }

    #[must_use]
    #[inline]
    fn gt(&self, other: &Rhs) -> bool
    {
        self.partial_cmp(&other) == Some(Ordering::Greater)
    }

    #[must_use]
    #[inline]
    fn ge(&self, other: &Rhs) -> bool
    {
        let res = self.partial_cmp(&other);
        res == Some(Ordering::Greater) || res == Some(Ordering::Equal)
    }
}


// Magic type to keep rustc happy.
pub struct AssertParamIsEq<T: Eq + ?Sized>
{
    _field: crate::marker::PhantomData<T>
}


#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics, derive_eq, structural_match)]
pub macro Eq($Item:item)
{
    /* builtin macro */
}

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics)]
pub macro Ord($Item:item)
{
    /* builtin marco */
}

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics, structural_match)]
pub macro PartialEq($Item:item)
{
    /* builtin macro */
}

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics)]
pub macro PartialOrd($Item:item)
{
    /* builtin macro */
}


macro_rules! partial_eq_impl
{
    ($($T:ty)+) =>
        ($(
            impl PartialEq<$T> for $T
            {
                #[inline]
                fn eq(&self, other: &$T) -> bool
                {
                    *self == *other
                }

                #[inline]
                fn ne(&self, other: &$T) -> bool
                {
                    *self != *other
                }
            }
        )+);
}

macro_rules! partial_ord_impl
{
    ($($T:ty)+) =>
        ($(
            partial_eq_impl!{$T}

            impl PartialOrd<$T> for $T
            {
                #[inline]
                fn partial_cmp(&self, other: &$T) -> Option<Ordering>
                {
                    if      *self < *other  { Some(Ordering::Less) }
                    else if *self > *other  { Some(Ordering::Greater) }
                    else if *self == *other { Some(Ordering::Equal) }
                    else                    { None }
                }

                #[inline]
                fn lt(&self, other: &$T) -> bool
                {
                    *self < *other
                }

                #[inline]
                fn le(&self, other: &$T) -> bool
                {
                    *self <= *other
                }

                #[inline]
                fn gt(&self, other: &$T) -> bool
                {
                    *self > *other
                }

                #[inline]
                fn ge(&self, other: &$T) -> bool
                {
                    *self >= *other
                }
            }
        )+);
}

macro_rules! ord_impl
{
    ($($T:ty)+) =>
        ($(
            partial_ord_impl!{$T}
            
            impl Eq for $T {}

            impl Ord for $T
            {
                #[inline]
                fn cmp(&self, other: &$T) -> Ordering
                {
                    if      *self < *other { Ordering::Less }
                    else if *self > *other { Ordering::Greater }
                    else                   { Ordering::Equal }
                }
            }
        )+);
}

partial_eq_impl!
{
    bool char
}

partial_ord_impl!
{
    f32 f64
}

ord_impl!
{
    i8 i16 i32 i64 i128
    u8 u16 u32 u64 u128
    isize usize
}

impl PartialEq<!> for !
{
    fn eq(&self, _other: &!) -> bool
    {
        true
    }
}

impl PartialEq<()> for ()
{
    fn eq(&self, _other: &()) -> bool
    {
        true
    }
}

impl PartialOrd<!> for !
{
    fn partial_cmp(&self, _other: &!) -> Option<Ordering>
    {
        Some(Ordering::Equal)
    }
}

impl PartialOrd<()> for ()
{
    fn partial_cmp(&self, _other: &()) -> Option<Ordering>
    {
        Some(Ordering::Equal)
    }
}

impl PartialOrd<char> for char
{
    fn partial_cmp(&self, other: &char) -> Option<Ordering>
    {
        (*self as u8).partial_cmp(&(*other as u8))
    }
}

impl Eq for bool {}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized, U: ?Sized> PartialEq<&U> for &T
where
    T: PartialEq<U>
{
    #[inline]
    fn eq(&self, other: &&U) -> bool
    {
        PartialEq::eq(*self, *other)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized, U: ?Sized> PartialEq<&mut U> for &mut T
where
    T: PartialEq<U>
{
    #[inline]
    fn eq(&self, other: &&mut U) -> bool
    {
        PartialEq::eq(*self, *other)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized + Eq> Eq for &T {}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized + Eq> Eq for &mut T {}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized, U: ?Sized> PartialOrd<&U> for &T
where
    T: PartialOrd<U>
{
    #[inline]
    fn partial_cmp(&self, other: &&U) -> Option<Ordering>
    {
        PartialOrd::partial_cmp(*self, *other)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl<T: ?Sized, U: ?Sized> PartialOrd<&mut U> for &mut T
where
    T: PartialOrd<U>
{
    #[inline]
    fn partial_cmp(&self, other: &&mut U) -> Option<Ordering>
    {
        PartialOrd::partial_cmp(*self, *other)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized + Ord> Ord for &T
{
    #[inline]
    fn cmp(&self, other: &&T) -> Ordering
    {
        Ord::cmp(*self, *other)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl <T: ?Sized + Ord> Ord for &mut T
{
    #[inline]
    fn cmp(&self, other: &&mut T) -> Ordering
    {
        Ord::cmp(*self, *other)
    }
}

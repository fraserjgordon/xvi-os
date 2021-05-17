pub enum Infallible
{
}


pub trait AsMut<T: ?Sized>
{
    fn as_mut(&mut self) -> &mut T;
}

pub trait AsRef<T: ?Sized>
{
    fn as_ref(&self) -> &T;
}

pub trait From<T>: Sized
{
    #[lang = "from"]
    fn from(_: T) -> Self;
}

pub trait Into<T> : Sized
{
    fn into(self) -> T;
}

pub trait TryFrom<T> : Sized
{
    type Error;

    fn try_from(value: T) -> Result<Self, Self::Error>;
}

pub trait TryInto<T> : Sized
{
    type Error;

    fn try_into(self) -> Result<T, Self::Error>;
}


pub const fn identity<T>(t: T) -> T
{
    t
}


impl <T> AsRef<[T]> for [T]
{
    #[inline]
    fn as_ref(&self) -> &[T]
    {
        self
    }
}

impl <T: ?Sized, U: ?Sized> AsRef<U> for &T
where
    T: AsRef<U>
{
    #[inline]
    fn as_ref(&self) -> &U
    {
        <T as AsRef<U>>::as_ref(*self)
    }
}

impl <T: ?Sized, U: ?Sized> AsRef<U> for &mut T
where
    T: AsRef<U>
{
    #[inline]
    fn as_ref(&self) -> &U
    {
        <T as AsRef<U>>::as_ref(*self)
    }
}

impl <T: ?Sized, U: ?Sized> AsMut<U> for &mut T
where
    T: AsMut<U>
{
    #[inline]
    fn as_mut(&mut self) -> &mut U
    {
        <T as AsMut<U>>::as_mut(*self)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl AsRef<str> for str
{
    #[inline]
    fn as_ref(&self) -> &str
    {
        self
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl AsMut<str> for str
{
    #[inline]
    fn as_mut(&mut self) -> &mut str
    {
        self
    }
}


impl <T> From<T> for T
{
    fn from(t: T) -> T
    {
        t
    }
}

impl <T, U> Into<U> for T
where
    U: From<T>
{
    fn into(self) -> U
    {
        U::from(self)
    }
}

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

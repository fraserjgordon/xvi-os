use crate::mem;
use crate::ops;

#[derive(Debug, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum Option<T>
{
    #[lang = "None"]
    None,

    #[lang = "Some"]
    Some(T),
}

#[derive(Debug, Clone)]
pub struct IntoIter<A>
{
    pub(crate) val: Option<A>
}

#[derive(Debug, Clone)]
pub struct Iter<'a, A: 'a>
{
    pub(crate) val: Option<&'a A>
}

#[derive(Debug)]
pub struct IterMut<'a, A: 'a>
{
    pub(crate) val: Option<&'a mut A>
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct NoneError;


impl <T> Option<T>
{
    #[must_use]
    #[inline]
    pub const fn is_some(&self) -> bool
    {
        match self
        {
            Some(_) => true,
            None => false
        }
    }

    #[must_use]
    #[inline]
    pub const fn is_none(&self) -> bool
    {
        !self.is_some()
    }

    #[must_use]
    #[inline]
    pub fn contains<U>(&self, u: &U) -> bool
    where
        U: PartialEq<T>,
    {
        match self
        {
            Some(x) => *u == *x,
            None => false,
        }
    }

    #[must_use]
    #[inline]
    pub const fn as_ref(&self) -> Option<&T>
    {
        match self
        {
            Some(x) => Some(&x),
            None => None,
        }
    }

    #[must_use]
    #[inline]
    pub fn as_mut(&mut self) -> Option<&mut T>
    {
        match self
        {
            Some(x) => Some(x),
            None => None,
        }
    }

    //pub fn as_pin_ref(self: Pin<&Self>) -> Option<Pin<T>>;

    //pub fn as_pin_mut(self: Pin<&mut Self>) -> Option<Pin<&mut T>>;

    #[inline]
    pub fn expect(self, msg: &str) -> T
    {
        match self
        {
            Some(x) => x,
            None => panic!(msg),
        }
    }

    #[inline]
    pub fn unwrap(self) -> T
    {
        match self
        {
            Some(x) => x,
            None => panic!(),
        }
    }

    #[inline]
    pub fn unwrap_or(self, default: T) -> T
    {
        match self
        {
            Some(x) => x,
            None => default,
        }
    }

    #[inline]
    pub fn unwrap_or_else<F: FnOnce() -> T>(self, f: F) -> T
    {
        match self
        {
            Some(x) => x,
            None => f(),
        }
    }

    #[inline]
    pub fn map<U, F: FnOnce(T) -> U>(self, f: F) -> Option<U>
    {
        match self
        {
            Some(x) => Some(f(x)),
            None => None,
        }
    }

    #[inline]
    pub fn map_or<U, F: FnOnce(T) -> U>(self, default: U, f: F) -> Option<U>
    {
        match self
        {
            Some(x) => Some(f(x)),
            None => Some(default),
        }
    }

    #[inline]
    pub fn map_or_else<U, D: FnOnce() -> U, F: FnOnce(T) -> U>(self, default: D, f: F) -> Option<U>
    {
        match self
        {
            Some(x) => Some(f(x)),
            None => Some(default()),
        }
    }

    #[inline]
    pub fn ok_or<E>(self, err: E) -> Result<T, E>
    {
        match self
        {
            Some(x) => Ok(x),
            None => Err(err),
        }
    }

    #[inline]
    pub fn ok_or_else<E, F: FnOnce() -> E>(self, err: F) -> Result<T, E>
    {
        match self
        {
            Some(x) => Ok(x),
            None => Err(err()),
        }
    }

    //pub fn iter(&self) -> Iter<'_, T>;

    //pub fn iter_mut(&mut self) -> IterMut<'_, T>;

    #[inline]
    pub fn and<U>(self, optb: Option<U>) -> Option<U>
    {
        match self
        {
            Some(_) => optb,
            None => None,
        }
    }

    #[inline]
    pub fn and_then<U, F: FnOnce(T) -> U>(self, f: F) -> Option<U>
    {
        match self
        {
            Some(x) => Some(f(x)),
            None => None,
        }
    }

    #[inline]
    pub fn filter<P: FnOnce(&T) -> bool>(self, predicate: P) -> Self
    {
        match self
        {
            Some(x) => if predicate(&x) { Some(x) } else { None },
            None => None,
        }
    }

    #[inline]
    pub fn or(self, optb: Option<T>) -> Option<T>
    {
        match self
        {
            Some(_) => self,
            None => optb,
        }
    }

    #[inline]
    pub fn or_else<F: FnOnce() -> Option<T>>(self, f: F) -> Option<T>
    {
        match self
        {
            Some(_) => self,
            None => f(),
        }
    }

    #[inline]
    pub fn xor(self, optb: Option<T>) -> Option<T>
    {
        match (self, optb)
        {
            (Some(_), Some(_)) => None,
            (Some(x), None)    => Some(x),
            (None, Some(x))    => Some(x),
            (None, None)       => None,
        }
    }

    #[inline]
    pub fn get_or_insert(&mut self, v: T) -> &mut T
    {
        if let None = *self
        {
            *self = Some(v)
        }

        match self
        {
            Some(x) => x,
            None => unsafe { crate::hints::unreachable_unchecked() },
        }
    }

    #[inline]
    pub fn get_or_insert_with<F: FnOnce() -> T>(&mut self, f: F) -> &mut T
    {
        if let None = *self
        {
            *self = Some(f())
        }

        match self
        {
            Some(x) => x,
            None => unsafe { crate::hints::unreachable_unchecked() },
        }
    }

    #[inline]
    pub fn take(&mut self) -> Option<T>
    {
        mem::take(self)
    }

    #[inline]
    pub fn replace(&mut self, t: T) -> Option<T>
    {
        mem::replace(self, Some(t))
    }

    #[inline]
    pub fn zip<U>(self, other: Option<U>) -> Option<(T, U)>
    {
        match (self, other)
        {
            (Some(x), Some(y)) => Some((x, y)),
            _ => None,
        }
    }

    #[inline]
    pub fn zip_with<U, F, R>(self, other: Option<U>, f: F) -> Option<R>
    where
        F: FnOnce(T, U) -> R
    {
        match (self, other)
        {
            (Some(x), Some(y)) => Some(f(x, y)),
            _ => None,
        }
    }
}

impl <'a, T: Copy> Option<&'a T>
{
    #[inline]
    pub fn copied(self) -> Option<T>
    {
        match self
        {
            Some(x) => Some(*x),
            None => None,
        }
    }
}

impl <'a, T: Copy> Option<&'a mut T>
{
    #[inline]
    pub fn copied(self) -> Option<T>
    {
        match self
        {
            Some(x) => Some(*x),
            None => None,
        }
    }
}

impl <'a, T: Clone> Option<&'a T>
{
    #[inline]
    pub fn cloned(self) -> Option<T>
    {
        match self
        {
            Some(x) => Some(x.clone()),
            None => None,
        }
    }
}

impl <'a, T: Clone> Option<&'a mut T>
{
    #[inline]
    pub fn cloned(self) -> Option<T>
    {
        match self
        {
            Some(x) => Some(x.clone()),
            None => None,
        }
    }
}

impl <T: crate::fmt::Debug> Option<T>
{
    #[inline]
    pub fn expect_none(self, msg: &str)
    {
        if let Some(x) = self
        {
            panic!("{}: {:?}", msg, x)
        }
    }

    #[inline]
    pub fn unwrap_none(self)
    {
        if let Some(x) = self
        {
            panic!("{:?}", x)
        }
    }
}

impl <T: Default> Option<T>
{
    #[inline]
    pub fn unwrap_or_default(self) -> T
    {
        match self
        {
            Some(x) => x,
            None => T::default(),
        }
    }
}

impl <T: ops::Deref> Option<T>
{
    #[inline]
    pub fn as_deref(&self) -> Option<&T::Target>
    {
        match self
        {
            Some(x) => Some(x.deref()),
            None => None,
        }
    }
}

impl <T: ops::DerefMut> Option<T>
{
    #[inline]
    pub fn as_deref_mut(&mut self) -> Option<&mut T::Target>
    {
        match self
        {
            Some(x) => Some(x.deref_mut()),
            None => None,
        }
    }
}

impl <T, E> Option<Result<T, E>>
{
    #[inline]
    pub fn transpose(self) -> Result<Option<T>, E>
    {
        match self
        {
            Some(Ok(x)) => Ok(Some(x)),
            Some(Err(x)) => Err(x),
            None => Ok(None),
        }
    }
}

impl <T> Option<Option<T>>
{
    #[inline]
    pub fn flatten(self) -> Option<T>
    {
        match self
        {
            Some(Some(x)) => Some(x),
            _ => None,
        }
    }
}


impl <T: Clone> Clone for Option<T>
{
    #[inline]
    fn clone(&self) -> Self
    {
        match self
        {
            Some(x) => Some(x.clone()),
            None => None,
        }
    }

    #[inline]
    fn clone_from(&mut self, source: &Self)
    {
        match (self, source)
        {
            (Some(to), Some(from)) => to.clone_from(from),
            (to, from) => *to = from.clone(),
        }
    }
}

impl <T> Default for Option<T>
{
    fn default() -> Option<T>
    {
        None
    }
}

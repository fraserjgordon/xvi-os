use crate::ops::{Deref, DerefMut};

#[must_use]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum Result<T, E>
{
    #[lang = "Ok"]
    Ok(T),

    #[lang = "Err"]
    Err(E)
}


#[derive(Debug, Clone)]
pub struct IntoIter<T>
{
    pub(crate) val: Option<T>
}

#[derive(Debug, Clone)]
pub struct Iter<'a, T: 'a>
{
    pub(crate) val: Option<&'a T>
}

#[derive(Debug)]
pub struct IterMut<'a, T: 'a>
{
    pub(crate) val: Option<&'a mut T>
}


impl <T, E> Result<T, E>
{
    #[must_use]
    #[inline]
    pub const fn is_ok(&self) -> bool
    {
        match self
        {
            Ok(_) => true,
            Err(_) => false,
        }
    }

    #[must_use]
    #[inline]
    pub const fn is_err(&self) -> bool
    {
        !self.is_ok()
    }

    #[must_use]
    #[inline]
    pub fn contains<U>(&self, t: &U) -> bool
    where
        U: PartialEq<T>
    {
        match self
        {
            Ok(x) => t.eq(x),
            Err(_) => false,
        }
    }

    #[must_use]
    #[inline]
    pub fn contains_err<F>(&self, f: &F) -> bool
    where
        F: PartialEq<E>
    {
        match self
        {
            Ok(_) => false,
            Err(x) => f.eq(x),
        }
    }

    #[inline]
    pub fn ok(self) -> Option<T>
    {
        match self
        {
            Ok(x) => Some(x),
            Err(_) => None,
        }
    }

    #[inline]
    pub fn err(self) -> Option<E>
    {
        match self
        {
            Ok(_) => None,
            Err(x) => Some(x),
        }
    }

    #[inline]
    pub const fn as_ref(&self) -> Result<&T, &E>
    {
        match self
        {
            Ok(x) => Ok(&x),
            Err(x) => Err(&x),
        }
    }

    #[inline]
    pub fn as_mut(&mut self) -> Result<&mut T, &mut E>
    {
        match *self
        {
            Ok(ref mut x) => Ok(x),
            Err(ref mut x) => Err(x),
        }
    }

    #[inline]
    pub fn map<U, F: FnOnce(T) -> U>(self, op: F) -> Result<U, E>
    {
        match self
        {
            Ok(x) => Ok(op(x)),
            Err(x) => Err(x),
        }
    }

    #[inline]
    pub fn map_or<U, F: FnOnce(T) -> U>(self, default: U, op: F) -> U
    {
        match self
        {
            Ok(x) => op(x),
            Err(_) => default,
        }
    }

    #[inline]
    pub fn map_or_else<U, D: FnOnce(E) -> U, F: FnOnce(T) -> U>(self, default: D, op: F) -> U
    {
        match self
        {
            Ok(x) => op(x),
            Err(x) => default(x),
        }
    }

    #[inline]
    pub fn map_err<F, O: FnOnce(E) -> F>(self, op: O) -> Result<T, F>
    {
        match self
        {
            Ok(x) => Ok(x),
            Err(x) => Err(op(x)),
        }
    }

    #[inline]
    pub fn iter(&self) -> Iter<'_, T>
    {
        match self
        {
            Ok(x) => Iter{ val: Some(&x) },
            Err(_) => Iter{ val: None },
        }
    }

    #[inline]
    pub fn iter_mut(&mut self) -> IterMut<'_, T>
    {
        match *self
        {
            Ok(ref mut x) => IterMut{ val: Some(x) },
            Err(_) => IterMut{ val: None },
        }
    }

    #[inline]
    pub fn and<U>(self, res: Result<U, E>) -> Result<U, E>
    {
        match self
        {
            Ok(x) => res,
            Err(x) => Err(x),
        }
    }

    #[inline]
    pub fn and_then<U, F: FnOnce(T) -> Result<U, E>>(self, op: F) -> Result<U, E>
    {
        match self
        {
            Ok(x) => op(x),
            Err(x) => Err(x),
        }
    }

    #[inline]
    pub fn or<F>(self, res: Result<T, F>) -> Result<T, F>
    {
        match self
        {
            Ok(x) => Ok(x),
            Err(x) => res,
        }
    }

    #[inline]
    pub fn or_else<F, O: FnOnce(E) -> Result<T, F>>(self, op: O) -> Result<T, F>
    {
        match self
        {
            Ok(x) => Ok(x),
            Err(x) => op(x),
        }
    }

    #[inline]
    pub fn unwrap_or(self, default: T) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(_) => default,
        }
    }

    #[inline]
    pub fn unwrap_or_else<F: FnOnce(E) -> T>(self, op: F) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(x) => op(x),
        }
    }
}

impl <'a, T: Copy, E> Result<&'a T, E>
{
    #[inline]
    pub fn copied(self) -> Result<T, E>
    {
        match self
        {
            Ok(x) => Ok(*x),
            Err(x) => Err(x),
        }
    }
}

impl <'a, T: Copy, E> Result<&'a mut T, E>
{
    #[inline]
    pub fn copied(self) -> Result<T, E>
    {
        match self
        {
            Ok(x) => Ok(*x),
            Err(x) => Err(x),
        }
    }
}

impl <'a, T: Clone, E> Result<&'a T, E>
{
    #[inline]
    pub fn cloned(self) -> Result<T, E>
    {
        match self
        {
            Ok(x) => Ok(x.clone()),
            Err(x) => Err(x),
        }
    }
}

impl <'a, T: Clone, E> Result<&'a mut T, E>
{
    #[inline]
    pub fn cloned(self) -> Result<T, E>
    {
        match self
        {
            Ok(x) => Ok(x.clone()),
            Err(x) => Err(x),
        }
    }
}

impl <T, E: crate::fmt::Debug> Result<T, E>
{
    #[inline]
    pub fn expect(self, msg: &str) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(x) => panic!("{}: {:?}", msg, x),
        }
    }

    #[inline]
    pub fn unwrap(self) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(x) => panic!("{:?}", x),
        }
    }
}

impl <T: crate::fmt::Debug, E> Result<T, E>
{
    #[inline]
    pub fn expect_err(self, msg: &str) -> E
    {
        match self
        {
            Ok(x) => panic!("{}: {:?}", msg, x),
            Err(x) => x,
        }
    }

    #[inline]
    pub fn unwrap_err(self) -> E
    {
        match self
        {
            Ok(x) => panic!("{:?}", x),
            Err(x) => x,
        }
    }
}

impl <T: Default, E> Result<T, E>
{
    #[inline]
    pub fn unwrap_or_default(self) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(_) => T::default(),
        }
    }
}

impl <T, E: Into<!>> Result<T, E>
{
    #[inline]
    pub fn into_ok(self) -> T
    {
        match self
        {
            Ok(x) => x,
            Err(x) => x.into(),
        }
    }
}

impl <T: Deref, E> Result<T, E>
{
    #[inline]
    pub fn as_deref(&self) -> Result<&T::Target, &E>
    {
        match self
        {
            Ok(x) => Ok(x.deref()),
            Err(x) => Err(x),
        }
    }
}

impl <T: DerefMut, E> Result<T, E>
{
    #[inline]
    pub fn as_deref_mut(&mut self) -> Result<&mut T::Target, &mut E>
    {
        match self
        {
            Ok(x) => Ok(x.deref_mut()),
            Err(x) => Err(x)
        }
    }
}

impl <T, E> Result<Option<T>, E>
{
    #[inline]
    pub fn transpose(self) -> Option<Result<T, E>>
    {
        match self
        {
            Ok(None) => None,
            Ok(Some(x)) => Some(Ok(x)),
            Err(x) => Some(Err(x)),
        }
    }
}

impl <T, E> Result<Result<T, E>, E>
{
    #[inline]
    pub fn flatten(self) -> Result<T, E>
    {
        match self
        {
            Ok(Ok(x)) => Ok(x),
            Ok(Err(x)) => Err(x),
            Err(x) => Err(x),
        }
    }
}


impl <T, E> IntoIterator for Result<T, E>
{
    type Item = T;
    type IntoIter = IntoIter<T>;

    #[inline]
    fn into_iter(self) -> IntoIter<T>
    {
        match self
        {
            Ok(x) => IntoIter{ val: Some(x) },
            Err(_) => IntoIter{ val: None },
        }
    }
}

impl <'a, T, E> IntoIterator for &'a Result<T, E>
{
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;

    #[inline]
    fn into_iter(self) -> Iter<'a, T>
    {
        self.iter()
    }
}

impl <'a, T, E> IntoIterator for &'a mut Result<T, E>
{
    type Item = &'a mut T;
    type IntoIter = IterMut<'a, T>;

    #[inline]
    fn into_iter(self) -> IterMut<'a, T>
    {
        self.iter_mut()
    }
}

impl <T> Iterator for IntoIter<T>
{
    type Item = T;
    
    #[inline]
    fn next(&mut self) -> Option<Self::Item>
    {
        self.val.take()
    }
}

impl <'a, T> Iterator for Iter<'a, T>
{
    type Item = &'a T;
    
    #[inline]
    fn next(&mut self) -> Option<Self::Item>
    {
        self.val.take()
    }
}

impl <'a, T> Iterator for IterMut<'a, T>
{
    type Item = &'a mut T;
    
    #[inline]
    fn next(&mut self) -> Option<Self::Item>
    {
        self.val.take()
    }
}

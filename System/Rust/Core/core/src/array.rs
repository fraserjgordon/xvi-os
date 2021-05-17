use crate::borrow::{Borrow, BorrowMut};
use crate::ops::{Index, IndexMut};
use crate::slice;

#[lang = "array"]
impl <T, const N: usize> [T; N]
{
    #[inline]
    pub fn map<F, U>(self, _f: F) -> [U; N]
    where
        F: FnMut(T) -> U
    {
        todo!()
    }

    #[inline]
    pub fn zip<U>(self, _rhs: [U; N]) -> [(T, U); N]
    {
        todo!()
    }

    #[inline]
    pub fn as_slice(&self) -> &[T]
    {
        &self[..]
    }

    #[inline]
    pub fn as_mut_slice(&mut self) -> &mut [T]
    {
        &mut self[..]
    }
}

impl <T, const N: usize> AsMut<[T]> for [T; N]
{
    #[inline]
    fn as_mut(&mut self) -> &mut [T]
    {
        &mut self[..]
    }
}

impl <T, const N: usize> AsRef<[T]> for [T; N]
{
    #[inline]
    fn as_ref(&self) -> &[T]
    {
        &self[..]
    }
}

impl <T, const N: usize> Borrow<[T]> for [T; N]
{
    #[inline]
    fn borrow(&self) -> &[T]
    {
        &self[..]
    }
}

impl <T, const N: usize> BorrowMut<[T]> for [T; N]
{
    #[inline]
    fn borrow_mut(&mut self) -> &mut [T]
    {
        &mut self[..]
    }
}

impl <T, I, const N: usize> Index<I> for [T; N]
where
    [T]: Index<I>
{
    type Output = <[T] as Index<I>>::Output;

    #[inline]
    fn index(&self, index: I) -> &Self::Output
    {
        Index::index(self as &[T], index)
    }
}

impl <T, I, const N: usize> IndexMut<I> for [T; N]
where
    [T]: IndexMut<I>
{
    #[inline]
    fn index_mut(&mut self, index: I) -> &mut Self::Output
    {
        IndexMut::index_mut(self as &mut [T], index)
    }
}

impl <A, B, const N: usize> PartialEq<[B; N]> for [A; N]
where
    A: PartialEq<B>,
{
    #[inline]
    fn eq(&self, other: &[B; N]) -> bool
    {
        self[..] == other[..]
    }
}

impl <A, B, const N: usize> PartialEq<[B]> for [A; N]
where
    A: PartialEq<B>,
{
    #[inline]
    fn eq(&self, other: &[B]) -> bool
    {
        self[..] == other[..]
    }
}

impl <'b, A, B, const N: usize> PartialEq<&'b [B]> for [A; N]
where
    A: PartialEq<B>,
{
    #[inline]
    fn eq(&self, other: &&'b [B]) -> bool
    {
        self[..] == other[..]
    }
}

impl <'b, A, B, const N: usize> PartialEq<&'b mut [B]> for [A; N]
where
    A: PartialEq<B>
{
    #[inline]
    fn eq(&self, other: &&'b mut [B]) -> bool
    {
        self[..] == other[..]
    }
}

impl <T, const N: usize> Eq for [T; N]
where
    T: Eq
{
}

impl <'a, T, const N: usize> IntoIterator for &'a [T; N]
{
    type Item = &'a T;
    type IntoIter = slice::Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter
    {
        self.as_slice().into_iter()
    }
}

impl <'a, T, const N: usize> IntoIterator for &'a mut [T; N]
{
    type Item = &'a mut T;
    type IntoIter = slice::IterMut<'a, T>;

    fn into_iter(self) -> Self::IntoIter
    {
        self.as_mut_slice().into_iter()
    }
}

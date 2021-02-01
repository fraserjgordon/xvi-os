use crate::slice;

#[lang = "array"]
impl <T, const N: usize> [T; N]
{
    #[inline]
    pub fn map<F, U>(self, f: F) -> [U; N]
    where
        F: FnMut(T) -> U
    {

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
    fn eq(&self, other: &[B]) -> bool
    {
        self[..] == other[..]
    }
}

impl <'b, A, B, const N: usize> PartialEq<&'b mut [B]> for [A; N]
where
    A: PartialEq<B>
{
    #[inline]
    fn eq(&self, other: &[B]) -> bool
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
        self.as_slice.into_iter()
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

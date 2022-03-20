use crate::iterator;
use crate::marker;
use crate::mem;
use crate::ops;
use crate::ptr;

pub unsafe trait SliceIndex<T: ?Sized>
{
    type Output: ?Sized;

    fn get(self, slice: &T) -> Option<&Self::Output>;

    fn get_mut(self, slice: &mut T) -> Option<&mut Self::Output>;

    unsafe fn get_unchecked(self, slice: *const T) -> *const Self::Output;

    unsafe fn get_unchecked_mut(self, slice: *mut T) -> *mut Self::Output;

    fn index(self, slice: &T) -> &Self::Output;

    fn index_mut(self, slice: &mut T) -> &mut Self::Output;
}


pub struct Iter<'a, T: 'a>
{
    begin:  ptr::NonNull<T>,
    end:    *const T,

    _lifetime: marker::PhantomData<&'a T>,
}

impl <'a, T> Iter<'a, T>
{
    pub fn as_slice(&self) -> &'a [T]
    {
        unsafe
        {
            let len = self.end.offset_from(self.begin.as_ptr()) as usize;
            &*ptr::slice_from_raw_parts(self.begin.as_ptr(), len)
        }
    }
}

pub struct IterMut<'a, T: 'a>
{
    begin:  ptr::NonNull<T>,
    end:    *const T,

    _lifetime: marker::PhantomData<&'a mut T>,
}


pub fn from_ref<T>(s: &T) -> &[T]
{
    unsafe { &*ptr::slice_from_raw_parts(s, 1) }
}

pub fn from_mut<T>(s: &mut T) -> &mut [T]
{
    unsafe { &mut *ptr::slice_from_raw_parts_mut(s, 1) }
}

pub unsafe fn from_raw_parts<'a, T>(data: *const T, len: usize) -> &'a [T]
{
    &*ptr::slice_from_raw_parts(data, len)
}

pub unsafe fn from_raw_parts_mut<'a, T>(data: *mut T, len: usize) -> &'a mut [T]
{
    &mut *ptr::slice_from_raw_parts_mut(data, len)
}


#[lang = "slice"]
impl <T> [T]
{
    #[inline]
    pub const fn len(&self) -> usize
    {
        unsafe
        {
            ptr::SliceRepr { rust: self }.raw.len
        }
    }

    #[inline]
    pub const fn is_empty(&self) -> bool
    {
        self.len() == 0
    }

    #[inline]
    pub fn first(&self) -> Option<&T>
    {
        if self.is_empty()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(self.get_unchecked(0))
            }
        }
    }

    #[inline]
    pub fn first_mut(&mut self) -> Option<&mut T>
    {
        if self.is_empty()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(self.get_unchecked_mut(0))
            }
        }
    }

    #[inline]
    pub fn last(&self) -> Option<&T>
    {
        if self.is_empty()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(self.get_unchecked(self.len() - 1))
            }
        }
    }

    #[inline]
    pub fn last_mut(&mut self) -> Option<&mut T>
    {
        if self.is_empty()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(self.get_unchecked_mut(self.len() - 1))
            }
        }
    }

    #[inline]
    pub fn split_first(&self) -> Option<(&T, &[T])>
    {
        if let [first, rest @ ..] = self
        {
            Some((first, rest))
        }
        else
        {
            None
        }
    }

    #[inline]
    pub fn split_first_mut(&mut self) -> Option<(&mut T, &mut [T])>
    {
        if let [first, rest @ ..] = self
        {
            Some((first, rest))
        }
        else
        {
            None
        }
    }

    #[inline]
    pub fn split_last(&self) -> Option<(&T, &[T])>
    {
        if let [rest @ .., last] = self
        {
            Some((last, rest))
        }
        else
        {
            None
        }
    }

    #[inline]
    pub fn split_last_mut(&mut self) -> Option<(&mut T, &mut [T])>
    {
        if let [rest @ .., last] = self
        {
            Some((last, rest))
        }
        else
        {
            None
        }
    }

    #[inline]
    pub fn get<I>(&self, index: I) -> Option<&<I as SliceIndex<[T]>>::Output>
    where
        I: SliceIndex<[T]>,
    {
        index.get(self)
    }

    #[inline]
    pub fn get_mut<I>(&mut self, index: I) -> Option<&mut <I as SliceIndex<[T]>>::Output>
    where
        I: SliceIndex<[T]>,
    {
        index.get_mut(self)
    }

    #[inline]
    pub unsafe fn get_unchecked<I>(&self, index: I) -> &<I as SliceIndex<[T]>>::Output
    where
        I: SliceIndex<[T]>,
    {
        &*index.get_unchecked(self)
    }

    #[inline]
    pub unsafe fn get_unchecked_mut<I>(&mut self, index: I) -> &mut <I as SliceIndex<[T]>>::Output
    where
        I: SliceIndex<[T]>,
    {
        &mut *index.get_unchecked_mut(self)
    }

    #[inline]
    pub const fn as_ptr(&self) -> *const T
    {
        (self as *const [T]) as *const T
    }

    #[inline]
    pub fn as_mut_ptr(&mut self) -> *mut T
    {
        (self as *mut [T]) as *mut T
    }

    #[inline]
    pub fn as_ptr_range(&self) -> ops::Range<*const T>
    {
        ops::Range{ start: self.as_ptr(), end: unsafe { self.as_ptr().add(self.len()) } }
    }

    #[inline]
    pub fn as_mut_ptr_range(&mut self) -> ops::Range<*mut T>
    {
        ops::Range{ start: self.as_mut_ptr(), end: unsafe { self.as_mut_ptr().add(self.len()) } }
    }

    #[inline]
    pub fn swap(&mut self, a: usize, b: usize)
    {
        if a >= self.len() || b >= self.len()
        {
            panic!()
        }

        unsafe
        {
            mem::swap(&mut *self.as_mut_ptr().add(a), &mut *self.as_mut_ptr().add(b))
        }
    }

    #[inline]
    pub fn iter(&self) -> Iter<'_, T>
    {
        todo!()
    }

    #[inline]
    pub fn iter_mut(&mut self) -> IterMut<'_, T>
    {
        todo!()
    }
}


unsafe impl <T> SliceIndex<[T]> for usize
{
    type Output = T;

    #[inline]
    fn get(self, slice: &[T]) -> Option<&Self::Output>
    {
        if self >= slice.len()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(&*self.get_unchecked(slice))
            }
        }
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut Self::Output>
    {
        if self >= slice.len()
        {
            None
        }
        else
        {
            unsafe
            {
                Some(&mut *self.get_unchecked_mut(slice))
            }
        }
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const T
    {
        slice.as_ptr().add(self)
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut T
    {
        slice.as_mut_ptr().add(self)
    }

    #[inline]
    fn index(self, slice: &[T]) -> &Self::Output
    {
        match self.get(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut Self::Output
    {
        match self.get_mut(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }
}

unsafe impl <T> SliceIndex<[T]> for ops::Range<usize>
{
    type Output = [T];

    #[inline]
    fn get(self, slice: &[T]) -> Option<&[T]>
    {
        if self.start > self.end || self.end > slice.len()
            { None }
        else
            { unsafe { Some(&*self.get_unchecked(slice)) } }
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut [T]>
    {
        if self.start > self.end || self.end > slice.len()
            { None }
        else
            { unsafe { Some(&mut *self.get_unchecked_mut(slice)) } }
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const [T]
    {
        ptr::slice_from_raw_parts(slice.as_ptr().add(self.start), self.end - self.start)
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut [T]
    {
        ptr::slice_from_raw_parts_mut(slice.as_mut_ptr().add(self.start), self.end - self.start)
    }

    #[inline]
    fn index(self, slice: &[T]) -> &[T]
    {
        match self.get(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut [T]
    {
        match self.get_mut(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }
}

unsafe impl <T> SliceIndex<[T]> for ops::RangeFrom<usize>
{
    type Output = [T];

    #[inline]
    fn get(self, slice: &[T]) -> Option<&[T]>
    {
        if self.start > slice.len()
            { None }
        else
            { unsafe { Some(&*self.get_unchecked(slice)) } }
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut [T]>
    {
        if self.start > slice.len()
            { None }
        else
            { unsafe { Some(&mut *self.get_unchecked_mut(slice)) } }
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const [T]
    {
        ptr::slice_from_raw_parts(slice.as_ptr().add(self.start), slice.len() - self.start)
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut [T]
    {
        ptr::slice_from_raw_parts_mut(slice.as_mut_ptr().add(self.start), slice.len() - self.start)
    }

    #[inline]
    fn index(self, slice: &[T]) -> &[T]
    {
        match self.get(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut [T]
    {
        match self.get_mut(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }
}

unsafe impl <T> SliceIndex<[T]> for ops::RangeFull
{
    type Output = [T];

    #[inline]
    fn get(self, slice: &[T]) -> Option<&[T]>
    {
        Some(slice)
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut [T]>
    {
        Some(slice)
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const [T]
    {
        slice
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut [T]
    {
        slice
    }

    #[inline]
    fn index(self, slice: &[T]) -> &[T]
    {
        slice
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut [T]
    {
        slice
    }
}

/*unsafe impl <T> SliceIndex<[T]> for ops::RangeInclusive<usize>
{
    type Output = [T];

    #[inline]
    fn get(self, slice: &[T]) -> Option<&[T]>
    {
        if self.start > self.end || self.end > slice.len()
            { None }
        else
            { unsafe { Some(&*self.get_unchecked(slice)) } }
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut [T]>
    {
        if self.start > self.end || self.end > slice.len()
            { None }
        else
            { unsafe { Some(&mut *self.get_unchecked_mut(slice)) } }
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const [T]
    {
        unsafe { ptr::slice_from_raw_parts(slice.as_ptr().add(self.start), self.end - self.start) }
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut [T]
    {
        unsafe { ptr::slice_from_raw_parts_mut(slice.as_mut_ptr().add(self.start), self.end - self.start) }
    }

    #[inline]
    fn index(self, slice: &[T]) -> &[T]
    {
        match self.get(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut [T]
    {
        match self.get_mut(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }
}*/

unsafe impl <T> SliceIndex<[T]> for ops::RangeTo<usize>
{
    type Output = [T];

    #[inline]
    fn get(self, slice: &[T]) -> Option<&[T]>
    {
        if self.end > slice.len()
            { None }
        else
            { unsafe { Some(&*self.get_unchecked(slice)) } }
    }

    #[inline]
    fn get_mut(self, slice: &mut [T]) -> Option<&mut [T]>
    {
        if self.end > slice.len()
            { None }
        else
            { unsafe { Some(&mut *self.get_unchecked_mut(slice)) } }
    }

    #[inline]
    unsafe fn get_unchecked(self, slice: *const [T]) -> *const [T]
    {
        ptr::slice_from_raw_parts(slice.as_ptr(), self.end)
    }

    #[inline]
    unsafe fn get_unchecked_mut(self, slice: *mut [T]) -> *mut [T]
    {
        ptr::slice_from_raw_parts_mut(slice.as_mut_ptr(), self.end)
    }

    #[inline]
    fn index(self, slice: &[T]) -> &[T]
    {
        match self.get(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }

    #[inline]
    fn index_mut(self, slice: &mut [T]) -> &mut [T]
    {
        match self.get_mut(slice)
        {
            Some(x) => x,
            None => panic!()
        }
    }
}

/*unsafe impl <T> SliceIndex<[T]> for ops::RangeToInclusive<usize>
{

}*/

impl <T, I> ops::Index<I> for [T]
where
    I: SliceIndex<[T]>
{
    type Output = <I as SliceIndex<[T]>>::Output;

    fn index(&self, index: I) -> &Self::Output
    {
        index.index(self)
    }
}

impl <T, I> ops::IndexMut<I> for [T]
where
    I: SliceIndex<[T]>
{
    fn index_mut(&mut self, index: I) -> &mut <I as SliceIndex<[T]>>::Output
    {
        index.index_mut(self)
    }
}


impl <'a, T> Iterator for Iter<'a, T>
{
    type Item = &'a T;

    fn next(&mut self) -> Option<&'a T>
    {
        todo!()
    }

    fn size_hint(&self) -> (usize, Option<usize>)
    {
        todo!()
    }

    fn count(self) -> usize
    {
        todo!()
    }

    fn nth(&mut self, n: usize) -> Option<&'a T>
    {
        todo!()
    }
}

impl <'a, T> Iterator for IterMut<'a, T>
{
    type Item = &'a mut T;

    fn next(&mut self) -> Option<&'a mut T>
    {
        todo!()
    }

    fn size_hint(&self) -> (usize, Option<usize>)
    {
        todo!()
    }

    fn count(self) -> usize
    {
        todo!()
    }

    fn nth(&mut self, n: usize) -> Option<&'a mut T>
    {
        todo!()
    }
}

impl <'a, T> iterator::IntoIterator for &'a [T]
{
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;

    fn into_iter(self) -> Iter<'a, T>
    {
        todo!()
    }
}

impl <'a, T> iterator::IntoIterator for &'a mut [T]
{
    type Item = &'a mut T;
    type IntoIter = IterMut<'a, T>;

    fn into_iter(self) -> IterMut<'a, T>
    {
        todo!()
    }
}

impl <A, B> PartialEq<[B]> for [A]
where
    A: PartialEq<B>
{
    fn eq(&self, other: &[B]) -> bool
    {
        if self.len() != other.len()
        {
            return false;
        }

        let n = self.len();
        let mut i = 0usize;

        while i < n
        {
            if self[i] != other[i]
            {
                return false;
            }

            i += 1;
        }

        return true;
    }
}

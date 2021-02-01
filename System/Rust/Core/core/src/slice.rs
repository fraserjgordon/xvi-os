use crate::marker;
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
        let len = self.end - self.begin.as_ptr();
        ptr::slice_from_raw_parts(self.begin.as_ptr(), len)
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
    ptr::slice_from_raw_parts(s, 1)
}

pub fn from_mut<T>(s: &mut T) -> &mut [T]
{
    ptr::slice_from_raw_parts_mut(s, 1)
}

pub unsafe fn from_raw_parts<'a, T>(data: *const T, len: usize) -> &'a [T]
{
    ptr::slice_from_raw_parts(data, len)
}

pub unsafe fn from_raw_parts_mut<'a, T>(data: *mut T, len: usize) -> &'a mut [T]
{
    ptr::slice_from_raw_parts_mut(data, len)
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
        unsafe
        {
            &*index.get_unchecked(self)
        }
    }

    #[inline]
    pub unsafe fn get_unchecked_mut<I>(&self, index: I) -> &mut <I as SliceIndex<[T]>>::Output
    where
        I: SliceIndex<[T]>,
    {
        unsafe
        {
            &mut *index.get_unchecked_mut(self)
        }
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

use crate::cmp;
use crate::intrinsics;
use crate::mem;

#[repr(transparent)]
pub struct NonNull<T: ?Sized>
{
    ptr: *const T,
}

#[repr(C)]
pub(crate) struct FatPtr<T>
{
    pub(crate) ptr: *const T,
    pub(crate) len: usize,
}

#[repr(C)]
pub(crate) union SliceRepr<T>
{
    pub(crate) rust:        *const [T],
    pub(crate) rust_mut:    *mut [T],
    pub(crate) raw:         FatPtr<T>,
}


#[inline]
pub unsafe fn copy<T>(src: *const T, dest: *mut T, count: usize)
{
    unsafe
    {
        intrinsics::copy(src, dest, count)
    }
}

#[inline]
pub unsafe fn copy_nonoverlapping<T>(src: *const T, dest: *mut T, count: usize)
{
    unsafe
    {
        intrinsics::copy_nonoverlapping(src, dest, count)
    }
}

#[lang = "drop_in_place"]
#[inline]
pub unsafe fn drop_in_place<T: ?Sized>(_: *mut T)
{
    /* compiler magic */
}

#[inline]
pub fn eq<T: ?Sized>(a: *const T, b: *const T) -> bool
{
    a == b
}

#[inline]
pub const fn null<T>() -> *const T
{
    0 as *const T
}

#[inline]
pub const fn null_mut<T>() -> *mut T
{
    0 as *mut T
}

#[inline]
pub unsafe fn read<T>(src: *const T) -> T
{
    let mut obj = mem::MaybeUninit::<T>::uninit();

    unsafe
    {
        intrinsics::copy_nonoverlapping(src, obj.as_mut_ptr(), 1);
        obj.assume_init()
    }
}

#[inline]
pub unsafe fn read_unaligned<T>(src: *const T) -> T
{
    let mut obj = mem::MaybeUninit::<T>::uninit();

    unsafe
    {
        let src_as_bytes = src as *const u8;
        let obj_as_bytes = obj.as_mut_ptr() as *mut u8;
        let size_in_bytes = mem::size_of::<T>();

        intrinsics::copy_nonoverlapping(src_as_bytes, obj_as_bytes, size_in_bytes);
        obj.assume_init()
    }
}

#[inline]
pub unsafe fn read_volatile<T>(src: *const T) -> T
{
    unsafe
    {
        intrinsics::volatile_load(src)
    }
}

#[inline]
pub unsafe fn replace<T>(dest: *mut T, src: T) -> T
{
    let mut obj = mem::MaybeUninit::<T>::uninit();

    unsafe
    {
        intrinsics::copy_nonoverlapping(dest, obj.as_mut_ptr(), 1);
        intrinsics::copy_nonoverlapping(&src, dest, 1);
        obj.assume_init()
    }
}

#[inline]
pub fn slice_from_raw_parts<T>(data: *const T, len: usize) -> *const [T]
{
    unsafe
    {
        (SliceRepr { raw: FatPtr { ptr: data, len: len }}).rust
    }
}

#[inline]
pub fn slice_from_raw_parts_mut<T>(data: *mut T, len: usize) -> *mut [T]
{
    unsafe
    {
        (SliceRepr { raw: FatPtr { ptr: data, len: len }}).rust_mut
    }
}

#[inline]
pub unsafe fn swap<T>(x: *mut T, y: *mut T)
{
    let temp = mem::MaybeUninit::<T>::uninit();

    unsafe
    {
        intrinsics::copy_nonoverlapping(x, temp.as_mut_ptr(), 1);
        intrinsics::copy(y, x, 1);
        intrinsics::copy_nonoverlapping(temp.as_ptr(), y, 1);
    }
}

#[inline]
pub unsafe fn swap_nonoverlapping<T>(x: *mut T, y: *mut T)
{
    let temp = mem::MaybeUninit::<T>::uninit();

    unsafe
    {
        intrinsics::copy_nonoverlapping(x, temp.as_mut_ptr(), 1);
        intrinsics::copy_nonoverlapping(y, x, 1);
        intrinsics::copy_nonoverlapping(temp.as_ptr(), y, 1);
    }
}

#[inline]
pub unsafe fn write<T>(dest: *mut T, src: T)
{
    unsafe
    {
        intrinsics::move_val_init(dest, src);
    }
}

#[inline]
pub unsafe fn write_bytes<T>(dest: *mut T, val: u8, count: usize)
{
    unsafe
    {
        intrinsics::write_bytes(dest, val, count);
    }
}

#[inline]
pub unsafe fn write_unaligned<T>(dest: *mut T, src: T)
{
    unsafe
    {
        intrinsics::copy_nonoverlapping(&src, dest, 1);
    }

    mem::forget(src);
}

#[inline]
pub unsafe fn write_volatile<T>(dest: *mut T, src: T)
{
    unsafe
    {
        intrinsics::volatile_store(dest, src);
    }
}


impl <T: Sized> NonNull<T>
{
    #[inline]
    pub const fn dangling() -> Self
    {
        NonNull { ptr: (mem::align_of::<T>() as *const T) }
    }

    #[inline]
    pub unsafe fn as_uninit_ref(&self) -> &mem::MaybeUninit<T>
    {
        unsafe
        {
            &*(self.cast::<mem::MaybeUninit<T>>().as_ptr())
        }
    }

    #[inline]
    pub unsafe fn as_uninit_mut(&mut self) -> &mut mem::MaybeUninit<T>
    {
        unsafe
        {
            &mut *(self.cast::<mem::MaybeUninit<T>>().as_ptr())
        }
    }
}

impl <T: ?Sized> NonNull<T>
{
    #[inline]
    pub const unsafe fn new_unchecked(ptr: *mut T) -> Self
    {
        NonNull { ptr: ptr }
    }

    #[inline]
    pub fn new(ptr: *mut T) -> Option<Self>
    {
        if ptr == (0 as *mut T)
        {
            None
        }
        else
        {
            Some(Self::new_unchecked(ptr))
        }
    }

    #[inline]
    pub const fn as_ptr(self) -> *mut T
    {
        self.ptr as *mut T
    }

    #[inline]
    pub unsafe fn as_ref(&self) -> &T
    {
        &*self.ptr
    }

    #[inline]
    pub unsafe fn as_mut(&mut self) -> &mut T
    {
        &mut *self.ptr
    }

    #[inline]
    pub const fn cast<U>(self) -> NonNull<U>
    {
        NonNull::<U>::new_unchecked(self.ptr as *mut U)
    }
}

impl <T> NonNull<[T]>
{
    #[inline]
    pub fn slice_from_raw_parts(data: NonNull<T>, len: usize) -> Self
    {
        NonNull { ptr: slice_from_raw_parts(data.ptr, len) }
    }

    #[inline]
    pub fn len(self) -> usize
    {
        self.as_ptr().len()
    }

    #[inline]
    pub fn as_non_null_ptr(self) -> NonNull<T>
    {
        NonNull::<T>::new_unchecked(self.as_ptr().as_mut_ptr())
    }

    #[inline]
    pub fn as_mut_ptr(self) -> *mut T
    {
        self.as_non_null_ptr().as_ptr()
    }

    #[inline]
    pub unsafe fn as_uninit_slice(&self) -> &[mem::MaybeUninit<T>]
    {
        &*(self.cast::<[mem::MaybeUninit<T>]>().as_ptr())
    }

    #[inline]
    pub unsafe fn as_uninit_mut_slice(&mut self) -> &mut [mem::MaybeUninit<T>]
    {
        &mut *(self.cast::<[mem::MaybeUninit<T>]>().as_ptr())
    }

    #[inline]
    pub unsafe fn get_unchecked_mut<I>(self, index: I) -> NonNull<I::Output>
    where
        I: crate::slice::SliceIndex<[T]>,
    {
        unsafe
        {
            NonNull::<I::Output>::new_unchecked(self.as_ptr().get_unchecked_mut(index))
        }
    }
}


impl <T: ?Sized> PartialEq for *const T
{
    #[inline]
    fn eq(&self, other: &Self) -> bool
    {
        *self == *other
    }
}

impl <T: ?Sized> PartialEq for *mut T
{
    #[inline]
    fn eq(&self, other: &Self) -> bool
    {
        *self == *other
    }
}

impl <T: ?Sized> Eq for *const T {}

impl <T: ?Sized> Eq for *mut T {}

impl <T: ?Sized> PartialOrd for *const T
{
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering>
    {
        if *self < *other
        {
            Some(cmp::Ordering::Less)
        }
        else if *self > *other
        {
            Some(cmp::Ordering::Greater)
        }
        else
        {
            Some(cmp::Ordering::Equal)
        }
    }
}

impl <T: ?Sized> PartialOrd for *mut T
{
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering>
    {
        if *self < *other
        {
            Some(cmp::Ordering::Less)
        }
        else if *self > *other
        {
            Some(cmp::Ordering::Greater)
        }
        else
        {
            Some(cmp::Ordering::Equal)
        }
    }
}

impl <T: ?Sized> Ord for *const T
{
    #[inline]
    fn cmp(&self, other: &Self) -> cmp::Ordering
    {
        if *self < *other
        {
            cmp::Ordering::Less
        }
        else if *self > *other
        {
            cmp::Ordering::Greater
        }
        else
        {
            cmp::Ordering::Equal
        }
    }
}

impl <T: ?Sized> Ord for *mut T
{
    #[inline]
    fn cmp(&self, other: &Self) -> cmp::Ordering
    {
        if *self < *other
        {
            cmp::Ordering::Less
        }
        else if *self > *other
        {
            cmp::Ordering::Greater
        }
        else
        {
            cmp::Ordering::Equal
        }
    }
}

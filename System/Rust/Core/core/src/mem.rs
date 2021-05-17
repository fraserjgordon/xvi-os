use crate::intrinsics;
use crate::ops::{Deref, DerefMut};
use crate::ptr;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Discriminant<T>(<T as crate::marker::DiscriminantKind>::Discriminant);

#[lang = "manually_drop"]
#[repr(transparent)]
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct ManuallyDrop<T: ?Sized>
{
    contents: T,
}

impl <T> ManuallyDrop<T>
{
    #[must_use]
    #[inline]
    pub const fn new(value: T) -> ManuallyDrop<T>
    {
        ManuallyDrop { contents: value }
    }

    #[inline]
    pub const fn into_inner(slot: ManuallyDrop<T>) -> T
    {
        slot.contents
    }

    #[inline]
    pub unsafe fn take(slot: &mut ManuallyDrop<T>) -> T
    {
        ptr::read(&slot.contents)
    }
}

impl <T: ?Sized> ManuallyDrop<T>
{
    #[inline]
    pub unsafe fn drop(slot: &mut ManuallyDrop<T>)
    {
        ptr::drop_in_place(&mut slot.contents)
    }
}

impl <T: ?Sized> Deref for ManuallyDrop<T>
{
    type Target = T;

    fn deref(&self) -> &T
    {
        &self.contents
    }
}

impl <T: ?Sized> DerefMut for ManuallyDrop<T>
{
    fn deref_mut(&mut self) -> &mut T
    {
        &mut self.contents
    }
}


#[lang = "maybe_uninit"]
#[repr(transparent)]
#[derive(Copy)]
pub union MaybeUninit<T>
{
    empty: (),
    value: ManuallyDrop<T>,
}

impl <T> MaybeUninit<T>
{
    pub const fn new(val: T) -> MaybeUninit<T>
    {
        MaybeUninit { value: ManuallyDrop { contents: val }}
    }

    pub const fn uninit() -> MaybeUninit<T>
    {
        MaybeUninit { empty: () }
    }

    pub fn uninit_array<const LEN: usize>() -> [Self; LEN]
    {
        unsafe
        {
            MaybeUninit::<[Self; LEN]>::uninit().assume_init()
        }
    }

    pub fn zeroed() -> MaybeUninit<T>
    {
        let mut obj = Self::uninit();

        unsafe
        {
            ptr::write_bytes(obj.as_mut_ptr(), 0, size_of::<T>())
        }

        obj
    }

    pub fn write(&mut self, val: T) -> &mut T
    {
        unsafe
        {
            ptr::write(self.as_mut_ptr(), val);
            &mut self.value.contents
        }
    }

    pub fn as_ptr(&self) -> *const T
    {
        unsafe
        {
            &self.value.contents
        }
    }

    pub fn as_mut_ptr(&mut self) -> *mut T
    {
        unsafe
        {
            &mut self.value.contents
        }
    }

    pub unsafe fn assume_init(self) -> T
    {
        self.value.contents
    }

    pub unsafe fn assume_init_read(&self) -> T
    {
        self.as_ptr().read()
    }

    pub unsafe fn assume_init_ref(&self) -> &T
    {
        &self.value.contents
    }

    pub unsafe fn slice_assume_init_ref(slice: &[Self]) -> &[T]
    {
        &*(slice as *const [Self] as *const [T])
    }

    pub unsafe fn slice_assume_ini_mut(slice: &mut [Self]) -> &[T]
    {
        &mut *(slice as *mut [Self] as *mut [T])
    }

    pub fn slice_as_ptr(this: &[MaybeUninit<T>]) -> *const T
    {
        this as *const [MaybeUninit<T>] as *const T
    }

    pub fn slice_as_mut_ptr(this: &mut [MaybeUninit<T>]) -> *mut T
    {
        this as *mut [MaybeUninit<T>] as *mut T
    }
}

impl <T: Copy> Clone for MaybeUninit<T>
{
    fn clone(&self) -> Self
    {
        *self
    }
}


#[inline]
pub const fn align_of<T>() -> usize
{
    intrinsics::min_align_of::<T>()
}

#[inline]
pub fn align_of_val<T: ?Sized>(val: &T) -> usize
{
    unsafe
    {
        intrinsics::min_align_of_val(val)
    }
}

#[inline]
pub fn discriminant<T>(v: &T) -> Discriminant<T>
{
    Discriminant ( intrinsics::discriminant_value(v) )
}

#[inline]
pub fn drop<T>(_: T)
{
}

#[inline]
pub const fn forget<T>(t: T)
{
    intrinsics::forget(t);
}

#[inline]
pub const fn needs_drop<T>() -> bool
{
    intrinsics::needs_drop::<T>()
}

#[must_use]
#[inline]
pub fn replace<T>(dest: &mut T, src: T) -> T
{
    unsafe
    {
        ptr::replace(dest, src)
    }
}

#[inline]
pub const fn size_of<T>() -> usize
{
    intrinsics::size_of::<T>()
}

#[inline]
pub fn size_of_val<T: ?Sized>(val: &T) -> usize
{
    unsafe
    {
        intrinsics::size_of_val(val)
    }
}

#[inline]
pub fn swap<T>(x: &mut T, y: &mut T)
{
    unsafe
    {
        ptr::swap_nonoverlapping(x, y)
    }
}

#[inline]
pub fn take<T: Default>(dest: &mut T) -> T
{
    replace(dest, T::default())
}

pub use intrinsics::transmute;

#[inline]
pub unsafe fn transmute_copy<T, U>(src: &T) -> U
{
    if align_of::<T>() < align_of::<U>()
    {
        ptr::read_unaligned(src as *const T as *const U)
    }
    else
    {
        ptr::read(src as *const T as *const U)
    }
}

#[inline]
pub fn variant_count<T>() -> usize
{
    intrinsics::variant_count::<T>()
}

#[inline]
pub unsafe fn zeroed<T>() -> T
{
    intrinsics::assert_zero_valid::<T>();
    MaybeUninit::<T>::zeroed().assume_init()
}

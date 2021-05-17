use crate::cmp;
use crate::intrinsics;
use crate::mem;
use crate::slice;

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
pub(crate) union PtrRepr<T: ?Sized>
{
    pub(crate) const_ptr:   *const T,
    pub(crate) mut_ptr:     *mut T,
    pub(crate) components:  PtrComponents<T>,
}

#[repr(C)]
pub(crate) struct PtrComponents<T: ?Sized>
{
    pub(crate) data:        *const (),
    pub(crate) metadata:    <T as Pointee>::Metadata,
}

#[repr(C)]
pub(crate) union SliceRepr<T>
{
    pub(crate) rust:        *const [T],
    pub(crate) rust_mut:    *mut [T],
    pub(crate) raw:         FatPtr<T>,
}


#[lang = "pointee_trait"]
pub trait Pointee
{
    #[lang = "metadata_type"]
    type Metadata: Copy + Send + Sync + Ord + crate::hash::Hash + Unpin;
}


#[inline]
pub unsafe fn copy<T>(src: *const T, dest: *mut T, count: usize)
{
    intrinsics::copy(src, dest, count)
}

#[inline]
pub unsafe fn copy_nonoverlapping<T>(src: *const T, dest: *mut T, count: usize)
{
    intrinsics::copy_nonoverlapping(src, dest, count)
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
pub fn metadata<T: ?Sized>(ptr: *const T) -> <T as Pointee>::Metadata
{
    unsafe
    {
        PtrRepr{ const_ptr: ptr }.components.metadata
    }
}

#[inline(always)]
#[rustc_diagnostic_item ="ptr_null"]
#[rustc_promotable]
#[rustc_const_stable(feature="always", since="0.0.0")]
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

    intrinsics::copy_nonoverlapping(src, obj.as_mut_ptr(), 1);
    obj.assume_init()
}

#[inline]
pub unsafe fn read_unaligned<T>(src: *const T) -> T
{
    let mut obj = mem::MaybeUninit::<T>::uninit();

    let src_as_bytes = src as *const u8;
    let obj_as_bytes = obj.as_mut_ptr() as *mut u8;
    let size_in_bytes = mem::size_of::<T>();

    intrinsics::copy_nonoverlapping(src_as_bytes, obj_as_bytes, size_in_bytes);
    obj.assume_init()
}

#[inline]
pub unsafe fn read_volatile<T>(src: *const T) -> T
{
    intrinsics::volatile_load(src)
}

#[inline]
pub unsafe fn replace<T>(dest: *mut T, src: T) -> T
{
    let mut obj = mem::MaybeUninit::<T>::uninit();

    intrinsics::copy_nonoverlapping(dest, obj.as_mut_ptr(), 1);
    intrinsics::copy_nonoverlapping(&src, dest, 1);
    obj.assume_init()
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
    let mut temp = mem::MaybeUninit::<T>::uninit();

    intrinsics::copy_nonoverlapping(x, temp.as_mut_ptr(), 1);
    intrinsics::copy(y, x, 1);
    intrinsics::copy_nonoverlapping(temp.as_ptr(), y, 1);
}

#[inline]
pub unsafe fn swap_nonoverlapping<T>(x: *mut T, y: *mut T)
{
    let mut temp = mem::MaybeUninit::<T>::uninit();

    intrinsics::copy_nonoverlapping(x, temp.as_mut_ptr(), 1);
    intrinsics::copy_nonoverlapping(y, x, 1);
    intrinsics::copy_nonoverlapping(temp.as_ptr(), y, 1);
}

#[inline]
pub unsafe fn write<T>(dest: *mut T, src: T)
{
    intrinsics::copy_nonoverlapping(&src, dest, 1);
    intrinsics::forget(src);
}

#[inline]
pub unsafe fn write_bytes<T>(dest: *mut T, val: u8, count: usize)
{
    intrinsics::write_bytes(dest, val, count);
}

#[inline]
pub unsafe fn write_unaligned<T>(dest: *mut T, src: T)
{
    intrinsics::copy_nonoverlapping(&src, dest, 1);
    mem::forget(src);
}

#[inline]
pub unsafe fn write_volatile<T>(dest: *mut T, src: T)
{
    intrinsics::volatile_store(dest, src);
}


impl <T: ?Sized> Copy for PtrComponents<T> {}

impl <T: ?Sized> Clone for PtrComponents<T>
{
    fn clone(&self) -> Self
    {
        *self
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
        &*self.cast().as_ptr()
    }

    #[inline]
    pub unsafe fn as_uninit_mut(&mut self) -> &mut mem::MaybeUninit<T>
    {
        &mut *(self.cast::<mem::MaybeUninit<T>>().as_ptr())
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
        if ptr.is_null()
        {
            None
        }
        else
        {
            unsafe { Some(Self::new_unchecked(ptr)) }
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
        &mut *(self.ptr as *mut T)
    }

    #[inline]
    pub const fn cast<U>(self) -> NonNull<U>
    {
        let ptr = self.ptr as *mut U;
        unsafe
        {
            NonNull::<U>::new_unchecked(ptr)
        }
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
        unsafe
        {
            NonNull::<T>::new_unchecked(self.as_ptr() as *mut T)
        }
    }

    #[inline]
    pub fn as_mut_ptr(self) -> *mut T
    {
        self.as_non_null_ptr().as_ptr()
    }

    #[inline]
    pub unsafe fn as_uninit_slice(&self) -> &[mem::MaybeUninit<T>]
    {
        slice::from_raw_parts(self.as_ptr() as *const mem::MaybeUninit<T>, self.len())
    }

    #[inline]
    pub unsafe fn as_uninit_mut_slice(&mut self) -> &mut [mem::MaybeUninit<T>]
    {
        slice::from_raw_parts_mut(self.as_mut_ptr() as *mut mem::MaybeUninit<T>, self.len())
    }

    #[inline]
    pub unsafe fn get_unchecked_mut<I>(self, index: I) -> NonNull<I::Output>
    where
        I: crate::slice::SliceIndex<[T]>,
    {
        NonNull::<I::Output>::new_unchecked(self.as_ptr().get_unchecked_mut(index))
    }
}

impl <T: ?Sized> Clone for NonNull<T>
{
    fn clone(&self) -> NonNull<T>
    {
        *self
    }
}

impl <T: ?Sized> Copy for NonNull<T>
{
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


#[lang = "const_ptr"]
impl <T> *const T
where
    T: ?Sized
{
    #[inline]
    pub fn is_null(self) -> bool
    {
        (self as *const u8).guaranteed_eq(null())
    }

    #[inline]
    pub const fn cast<U>(self) -> *const U
    {
        self as *const U
    }

    #[inline]
    pub unsafe fn as_ref<'a>(self) -> Option<&'a T>
    {
        if self.is_null()
            { None }
        else
            { Some(&*self) }
    }

    #[inline]
    pub unsafe fn as_uninit_ref<'a>(self) -> Option<&'a mem::MaybeUninit<T>>
    where
        T: Sized
    {
        if self.is_null()
            { None }
        else
            { Some(&*(self as *const mem::MaybeUninit<T>)) } 
    }

    #[inline]
    #[must_use]
    pub unsafe fn offset(self, count: isize) -> *const T
    where
        T: Sized
    {
        intrinsics::offset(self, count)
    }

    #[inline]
    #[must_use]
    pub fn wrapping_offset(self, count: isize) -> *const T
    where
        T: Sized
    {
        unsafe { intrinsics::arith_offset(self, count) }
    }

    #[inline]
    pub unsafe fn offset_from(self, origin: *const T) -> isize
    where
        T: Sized
    {
        intrinsics::ptr_offset_from(self, origin)
    }
    
    #[inline]
    pub fn guaranteed_eq(self, other: *const T) -> bool
    where
        T: Sized
    {
        intrinsics::ptr_guaranteed_eq(self, other)
    }

    #[inline]
    pub unsafe fn guaranteed_ne(self, other: *const T) -> bool
    where
        T: Sized
    {
        intrinsics::ptr_guaranteed_ne(self, other)
    }

    #[inline]
    #[must_use]
    pub unsafe fn add(self, count: usize) -> *const T
    where
        T: Sized
    {
        self.offset(count as isize)
    }

    #[inline]
    #[must_use]
    pub unsafe fn sub(self, count: usize) -> *const T
    where
        T: Sized
    {
        self.offset((count as isize).wrapping_neg())
    }

    #[inline]
    #[must_use]
    pub fn wrapping_add(self, count: usize) -> *const T
    where
        T: Sized
    {
        self.wrapping_offset(count as isize)
    }

    #[inline]
    #[must_use]
    pub fn wrapping_sub(self, count: usize) -> *const T
    where
        T: Sized
    {
        self.wrapping_offset((count as isize).wrapping_neg())
    }

    #[inline]
    #[must_use]
    pub fn set_ptr_value(self, val: *const u8) -> *const T
    {
        let as_thin_ptr = self as *mut *const T as *mut *const u8;
        unsafe { *as_thin_ptr = val }
        self
    }

    #[inline]
    pub unsafe fn read(self) -> T
    where
        T: Sized
    {
        read(self)
    }

    #[inline]
    pub unsafe fn read_volatile(self) -> T
    where
        T: Sized
    {
        read_volatile(self)
    }

    #[inline]
    pub unsafe fn read_unaligned(self) -> T
    where
        T: Sized
    {
        read_unaligned(self)
    }

    #[inline]
    pub unsafe fn copy_to(self, dest: *mut T, count: usize)
    where
        T: Sized
    {
        copy(self, dest, count)
    }

    #[inline]
    pub unsafe fn copy_to_nonoverlapping(self, dest: *mut T, count: usize)
    where
        T: Sized
    {
        copy_nonoverlapping(self, dest, count)
    }

    #[inline]
    pub fn align_offset(self, align: usize) -> usize
    where
        T: Sized
    {
        if !align.is_power_of_two()
            { panic!("invalid alignment") }

        let raw_ptr_val = self as *const u8 as usize;
        let ptr_val_in_elems = raw_ptr_val / intrinsics::size_of::<T>();
        let adjust = (ptr_val_in_elems + align - 1) & !(align - 1);

        adjust
    }
}

#[lang = "mut_ptr"]
impl <T> *mut T
where
    T: ?Sized
{
    #[inline]
    pub fn is_null(self) -> bool
    {
        (self as *mut u8).guaranteed_eq(null_mut())
    }

    #[inline]
    pub const fn cast<U>(self) -> *mut U
    {
        self as *mut U
    }

    #[inline]
    pub unsafe fn as_ref<'a>(self) -> Option<&'a T>
    {
        (self as *const T).as_ref::<'a>()
    }

    #[inline]
    pub unsafe fn as_uninit_ref<'a>(self) -> Option<&'a mem::MaybeUninit<T>>
    where
        T: Sized
    {
        (self as *const T).as_uninit_ref::<'a>()
    }

    #[inline]
    #[must_use]
    pub unsafe fn offset(self, count: isize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).offset(count) as *mut T
    }

    #[inline]
    #[must_use]
    pub fn wrapping_offset(self, count: isize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).wrapping_offset(count) as *mut T
    }

    #[inline]
    pub unsafe fn as_mut<'a>(self) -> Option<&'a mut T>
    {
        if self.is_null()
            { None }
        else
            { Some(&mut *self) }
    }

    #[inline]
    pub unsafe fn as_uninit_mut<'a>(self) -> Option<&'a mut mem::MaybeUninit<T>>
    where
        T: Sized
    {
        if self.is_null()
            { None }
        else
            { Some(&mut *(self as *mut mem::MaybeUninit<T>)) }
    }

    #[inline]
    pub fn guaranteed_eq(self, other: *mut T) -> bool
    where
        T: Sized
    {
        intrinsics::ptr_guaranteed_eq(self, other)
    }

    #[inline]
    pub unsafe fn guaranteed_ne(self, other: *mut T) -> bool
    where
        T: Sized
    {
        intrinsics::ptr_guaranteed_ne(self, other)
    }

    #[inline]
    pub unsafe fn offset_from(self, other: *mut T) -> isize
    where
        T: Sized
    {
        (self as *const T).offset_from(other)
    }

    #[inline]
    #[must_use]
    pub unsafe fn add(self, count: usize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).add(count) as *mut T
    }

    #[inline]
    #[must_use]
    pub unsafe fn sub(self, count: usize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).sub(count) as *mut T
    }

    #[inline]
    #[must_use]
    pub fn wrapping_add(self, count: usize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).wrapping_add(count) as *mut T
    }

    #[inline]
    #[must_use]
    pub fn wrapping_sub(self, count: usize) -> *mut T
    where
        T: Sized
    {
        (self as *const T).wrapping_sub(count) as *mut T
    }

    #[inline]
    #[must_use]
    pub fn set_ptr_value(self, val: *mut u8) -> *mut T
    {
        (self as *const T).set_ptr_value(val) as *mut T
    }

    #[inline]
    pub unsafe fn read(self) -> T
    where
        T: Sized
    {
        (self as *const T).read()
    }

    #[inline]
    pub unsafe fn read_volatile(self) -> T
    where
        T: Sized
    {
        (self as *const T).read_volatile()
    }

    #[inline]
    pub unsafe fn read_unaligned(self) -> T
    where
        T: Sized
    {
        (self as *const T).read_unaligned()
    }

    #[inline]
    pub unsafe fn copy_to(self, dest: *mut T, count: usize)
    where
        T: Sized
    {
        (self as *const T).copy_to(dest, count)
    }

    #[inline]
    pub unsafe fn copy_to_nonoverlapping(self, dest: *mut T, count: usize)
    where
        T: Sized
    {
        (self as *const T).copy_to_nonoverlapping(dest, count)
    }

    #[inline]
    pub unsafe fn copy_from(self, source: *const T, count: usize)
    where
        T: Sized
    {
        source.copy_to(self, count)
    }

    #[inline]
    pub unsafe fn copy_from_nonoverlapping(self, source: *const T, count: usize)
    where
        T: Sized
    {
        source.copy_to_nonoverlapping(self, count)
    }

    #[inline]
    pub unsafe fn drop_in_place(self)
    {
        drop_in_place(self)
    }

    #[inline]
    pub unsafe fn write(self, val: T)
    where
        T: Sized
    {
        write(self, val)
    }

    #[inline]
    pub unsafe fn write_bytes(self, val: u8, count: usize)
    where
        T: Sized
    {
        write_bytes(self, val, count)
    }

    #[inline]
    pub unsafe fn write_volatile(self, val: T)
    where
        T: Sized
    {
        write_volatile(self, val)
    }

    #[inline]
    pub unsafe fn write_unaligned(self, val: T)
    where
        T: Sized
    {
        write_unaligned(self, val)
    }

    #[inline]
    pub unsafe fn replace(self, val: T) -> T
    where
        T: Sized
    {
        replace(self, val)
    }

    #[inline]
    pub unsafe fn swap(self, other: *mut T)
    where
        T: Sized
    {
        swap(self, other)
    }

    #[inline]
    pub fn align_offset(self, align: usize) -> usize
    where
        T: Sized
    {
        (self as *const T).align_offset(align)
    }
}

#[lang = "const_slice_ptr"]
impl <T> *const [T]
{
    #[inline]
    pub fn len(self) -> usize
    {
        metadata(self)
    }
    
    #[inline]
    pub fn as_ptr(self) -> *const T
    {
        self as *const T
    }

    #[inline]
    pub unsafe fn get_unchecked<I>(self, index: I) -> *const <I as slice::SliceIndex<[T]>>::Output
    where
        I: slice::SliceIndex<[T]>
    {
        index.get_unchecked(self)
    }

    #[inline]
    pub unsafe fn as_uninit_slice<'a>(self) -> Option<&'a [mem::MaybeUninit<T>]>
    {
        if self.is_null()
            { None }
        else
            { Some(slice::from_raw_parts(&*(self as *const mem::MaybeUninit<T>), self.len())) }
    }
}

#[lang = "mut_slice_ptr"]
impl <T> *mut [T]
{
    #[inline]
    pub fn len(self) -> usize
    {
        (self as *const [T]).len()
    }

    #[inline]
    pub fn as_mut_ptr(self) -> *mut T
    {
        self as *mut T
    }

    #[inline]
    pub unsafe fn get_unchecked_mut<I>(self, index: I) -> *mut <I as slice::SliceIndex<[T]>>::Output
    where
        I: slice::SliceIndex<[T]>
    {
        index.get_unchecked_mut(self)
    }

    #[inline]
    pub unsafe fn as_uninit_slice<'a>(self) -> Option<&'a [mem::MaybeUninit<T>]>
    {
        (self as *const [T]).as_uninit_slice()
    }

    #[inline]
    pub unsafe fn as_uninit_slice_mut<'a>(self) -> Option<&'a mut [mem::MaybeUninit<T>]>
    {
        if self.is_null()
            { None }
        else
            { Some(slice::from_raw_parts_mut(&mut *(self as *mut mem::MaybeUninit<T>), self.len())) }
    }
}

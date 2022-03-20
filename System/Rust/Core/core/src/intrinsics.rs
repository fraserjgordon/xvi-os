use crate::marker::*;

extern "rust-intrinsic"
{

pub fn abort() -> !;

#[must_use]
pub fn arith_offset<T>(ptr: *const T, offset: isize) -> *const T;

pub fn assert_inhabited<T>();

pub fn assert_uninit_valid<T>();

pub fn assert_zero_valid<T>();

pub fn assume(b: bool);

pub fn breakpoint();

#[rustc_const_unstable(feature = "const_caller_location", issue = "none")]
pub fn caller_location() -> &'static crate::panic::Location<'static>;

pub fn discriminant_value<T>(v: &T) -> <T as DiscriminantKind>::Discriminant;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn forget<T: ?Sized>(x: T);

pub fn likely(b: bool) -> bool;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn min_align_of<T>() -> usize;

pub fn min_align_of_val<T: ?Sized>(p: *const T) -> usize;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn needs_drop<T>() -> bool;

pub fn nontemporal_store<T>(ptr: *mut T, val: T);

#[must_use]
pub fn offset<T>(ptr: *const T, offset: isize) -> *const T;

pub fn pref_align_of<T>() -> usize;

pub fn prefetch_read_data<T>(data: *const T, locality: i32);

pub fn prefetch_read_instruction<T>(data: *const T, locality: i32);

pub fn prefetch_write_data<T>(data: *const T, locality: i32);

pub fn prefetch_write_instruction<T>(data: *const T, locality: i32);

pub fn ptr_guaranteed_eq<T>(x: *const T, y: *const T) -> bool;

pub fn ptr_guaranteed_ne<T>(x: *const T, y: *const T) -> bool;

pub fn ptr_offset_from<T>(ptr: *const T, base: *const T) -> isize;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn size_of<T>() -> usize;

pub fn size_of_val<T: ?Sized>(ptr: *const T) -> usize;

pub fn transmute<T, U>(e: T) -> U;

pub fn r#try(try_fn: fn(_: *mut u8), data: *mut u8, catch_fn: fn(_: *mut u8, _: *mut u8)) -> i32;

pub fn type_id<T: ?Sized + 'static>() -> u64;

#[rustc_const_stable(feature="???", since="0.0.0")]
pub fn type_name<T: ?Sized>() -> &'static str;

pub fn unlikely(b: bool) -> bool;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn unreachable() -> !;

pub fn variant_count<T>() -> usize;

pub fn volatile_copy_memory<T>(dest: *mut T, src: *const T, count: usize);

pub fn volatile_copy_nonoverlapping_memory<T>(dest: *mut T, src: *const T, count: usize);

pub fn volatile_load<T>(ptr: *const T) -> T;

pub fn volatile_set_memory<T>(ptr: *mut T, val: u8, count: usize);

pub fn volatile_store<T>(ptr: *mut T, val: T);


pub fn bitreverse<T: Copy>(x: T) -> T;
pub fn bswap<T: Copy>(x: T) -> T;
pub fn ctlz<T: Copy>(x: T) -> T;
pub fn ctlz_nonzero<T: Copy>(x: T) -> T;
pub fn ctpop<T: Copy>(x: T) -> T;
pub fn cttz<T: Copy>(x: T) -> T;
pub fn cttz_nonzero<T: Copy>(x: T) -> T;
pub fn rotate_left<T: Copy>(x: T, y: T) -> T;
pub fn rotate_right<T: Copy>(x: T, y: T) -> T;

pub fn exact_div<T: Copy>(x: T, y: T) -> T;

pub fn ceilf32(x: f32) -> f32;
pub fn ceilf64(x: f64) -> f64;
pub fn copysignf32(x: f32, y: f32) -> f32;
pub fn copysignf64(x: f64, y: f64) -> f64;
pub fn cosf32(x: f32) -> f32;
pub fn cosf64(x: f64) -> f64;

pub fn add_with_overflow<T: Copy>(x: T, y: T) -> (T, bool);
pub fn mul_with_overflow<T: Copy>(x: T, y: T) -> (T, bool);
pub fn sub_with_overflow<T: Copy>(x: T, y: T) -> (T, bool);

pub fn saturating_add<T: Copy>(x: T, y: T) -> T;
pub fn saturating_sub<T: Copy>(x: T, y: T) -> T;

pub fn unchecked_add<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_div<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_mul<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_rem<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_shl<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_shr<T: Copy>(x: T, y: T) -> T;
pub fn unchecked_sub<T: Copy>(x: T, y: T) -> T;

#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn wrapping_add<T: Copy>(x: T, y: T) -> T;
#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn wrapping_mul<T: Copy>(x: T, y: T) -> T;
#[rustc_const_stable(feature="always", since="0.0.0")]
pub fn wrapping_sub<T: Copy>(x: T, y: T) -> T;

}

pub unsafe fn copy<T>(src: *const T, dest: *mut T, count: usize)
{
    extern "rust-intrinsic" { fn copy<T>(src: *const T, dest: *mut T, count: usize); }

    copy(src, dest, count)
}

pub unsafe fn copy_nonoverlapping<T>(src: *const T, dest: *mut T, count: usize)
{
    extern "rust-intrinsic" { fn copy_nonoverlapping<T>(src: *const T, dest: *mut T, count: usize); }

    copy_nonoverlapping(src, dest, count)
}

pub unsafe fn write_bytes<T>(dest: *mut T, val: u8, count: usize)
{
    extern "rust-intrinsic" { fn write_bytes<T>(dest: *mut T, val: u8, count: usize); }

    write_bytes(dest, val, count)
}

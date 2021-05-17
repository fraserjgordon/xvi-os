use crate::fmt::Debug;

#[lang = "copy"]
pub trait Copy: Clone {}

#[lang = "discriminant_kind"]
pub trait DiscriminantKind
{
    #[lang = "discriminant_type"]
    type Discriminant: Clone + Copy + Debug + Eq + PartialEq + crate::hash::Hash + Send + Sync + Unpin;
}

pub unsafe auto trait Send {}

#[lang = "sized"]
#[fundamental]
pub trait Sized {}

#[lang = "structural_teq"]
pub trait StructuralEq {}

#[lang = "structural_peq"]
pub trait StructuralPartialEq {}

#[lang = "sync"]
pub unsafe auto trait Sync {}

#[lang = "unpin"]
pub unsafe auto trait Unpin {}

#[lang = "unsize"]
pub trait Unsize<T: ?Sized> {}

#[lang = "freeze"]
pub(crate) unsafe auto trait Freeze {}


#[lang = "phantom_data"]
pub struct PhantomData<T: ?Sized>;

#[derive(Debug, Default, Clone, Copy, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct PhantomPinned;


#[rustc_builtin_macro]
pub macro Copy($Item:item)
{
    /* builtin macro */
}


mod impls
{

use super::*;

// Copy implementations for primitive types are provided by the compiler but
// still need to be declared.
macro_rules! primitive_copy_impl
{
    ($($T:ty)*) =>
        {
            $(impl Copy for $T {})*
        }
}

primitive_copy_impl!
{
    !
    bool
    char
    f32 f64
    i8 i16 i32 i64 i128
    u8 u16 u32 u64 u128
    isize usize
}

// Other types having compiler-based copies.
impl <T: ?Sized> Copy for *const T {}
impl <T: ?Sized> Copy for *mut T {}
impl <T: ?Sized> Copy for &'_ T {}

// Non-Send types.
impl <T: ?Sized> !Send for *const T {}
impl <T: ?Sized> !Send for *mut T {}

// Non-Sync types.
impl <T: ?Sized> !Sync for *const T{}
impl <T: ?Sized> !Sync for *mut T{}

// Non-Unpin types.
impl !Unpin for PhantomPinned {}

// Unpin impls.
unsafe impl <'a, T: ?Sized + 'a> Unpin for &'a T {}
unsafe impl <'a, T: ?Sized + 'a> Unpin for &'a mut T {}
unsafe impl <T: ?Sized> Unpin for *const T {}
unsafe impl <T: ?Sized> Unpin for *mut T {}

// Freeze impls.
unsafe impl <T: ?Sized> Freeze for PhantomData<T> {}
unsafe impl <T: ?Sized> Freeze for *const T {}
unsafe impl <T: ?Sized> Freeze for *mut T {}
unsafe impl <T: ?Sized> Freeze for &T {}
unsafe impl <T: ?Sized> Freeze for &mut T{}

}

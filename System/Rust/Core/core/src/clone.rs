#[lang = "clone"]
pub trait Clone: Sized
{
    fn clone(&self) -> Self;

    fn clone_from(&mut self, source: &Self)
    {
        *self = source.clone();
    }
}

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics, derive_clone_copy)]
pub macro Clone($Item:item)
{
    /* builtin macro */
}


// Magic type to keep rustc happy.
pub struct AssertParamIsClone<T: Clone + ?Sized>
{
    _field: crate::marker::PhantomData<T>
}


mod impls
{

use super::*;

// Primitive types have compiler-generated implementations of Clone.
macro_rules! primitive_clone_impl
{
    ($($T:ty)*) =>
        {
            $(impl Clone for $T
            {
                fn clone(&self) -> $T
                {
                    *self
                }
            })*
        }
}

primitive_clone_impl!
{
    !
    bool
    char
    f32 f64
    i8 i16 i32 i64 i128
    u8 u16 u32 u64 u128
    isize usize
}

// Other types with compiler-generated Clone implementations.
impl <T: ?Sized> Clone for *const T { fn clone(&self) -> Self { *self }}
impl <T: ?Sized> Clone for *mut T { fn clone(&self) -> Self { *self }}
impl <T: ?Sized> Clone for &'_ T { fn clone(&self) -> Self { *self }}

// Built-in types that are not cloneable.
impl <T: ?Sized> !Clone for &'_ mut T {}

}
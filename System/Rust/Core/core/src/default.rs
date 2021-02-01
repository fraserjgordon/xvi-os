pub trait Default: Sized
{
    fn default() -> Self;
}

#[rustc_builtin_macro]
pub macro Default($Item:item)
{
    /* builtin macro */
}

macro_rules! default_impl
{
    ($T:ty, $V:expr) =>
        (
            impl Default for $T
            {
                #[inline]
                fn default() -> $T
                {
                    $V
                }
            }
        )
}

macro_rules! default_impl_nums
{
    ($($T:ty)*) =>
        ($(
            default_impl!{$T, (0 as $T)}
        )*)
}

default_impl!{(), ()}
default_impl!{bool, false}
default_impl!{char, '\x00'}

default_impl_nums!
{
    f32 f64
    i8 i16 i32 i64 i128
    u8 u16 u32 u64 u128
    isize usize
}

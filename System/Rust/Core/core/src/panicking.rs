#![unstable(feature="core_panic", issue="none")]

use crate::fmt;
use crate::panic;

#[lang = "panic"]
#[track_caller]
pub fn panic(expr: &'static str) -> !
{
    extern "Rust"
    {
        #[lang = "panic_impl"]
        fn panic_impl(pi: &panic::PanicInfo<'_>) -> !;
    }

    unsafe
    {
        crate::intrinsics::unreachable();
    }
}

#[inline]
#[lang = "panic_str"]
#[track_caller]
pub fn panic_str(expr: &str) -> !
{
    unsafe
    {
        panic_fmt(format_args!("{}", expr));
    }
}

#[track_caller]
#[lang = "panic_fmt"]
pub fn panic_fmt(fmt: fmt::Arguments<'_>) -> !
{
    extern "Rust"
    {
        #[lang = "panic_impl"]
        fn panic_impl(pi: &panic::PanicInfo<'_>) -> !;
    }

    unsafe
    {
        panic_impl(&panic::PanicInfo::construct(Some(&fmt), panic::Location::caller()))
    }
}

#[lang = "const_panic_fmt"]
pub const fn const_panic_fmt(fmt: fmt::Arguments<'_>) -> !
{
    /*if let Some(message) = fmt.as_str()
    {
        panic_str(message);
    }*/

    unsafe
    {
        crate::hint::unreachable_unchecked();
    }
}

#[inline]
#[track_caller]
pub fn panic_display<T: fmt::Display>(x: &T) -> !
{
    unsafe
    {
        panic_fmt(format_args!("{}", *x));
    }
}

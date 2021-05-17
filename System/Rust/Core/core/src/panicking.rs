#![unstable(feature="core_panic", issue="none")]

use crate::panic;

#[lang = "panic"]
#[track_caller]
#[unstable(feature="core_panic", issue="none")]
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

/*pub fn panic_fmt(fmt: Arguments) -> !
{
    extern "Rust"
    {

    }
}*/

//pub fn panic_fmt(fmt: Arguments<'_>) -> !;

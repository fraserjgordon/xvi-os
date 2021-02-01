#[lang = "panic"]
pub fn panic(expr: &'static str) -> !
{
    extern "Rust"
    {
        //#[lang = "panic_impl"]
        //fn panic_impl(pi: &PanicInfo<'_>) -> !;
    }
}

/*pub fn panic_fmt(fmt: Arguments) -> !
{
    extern "Rust"
    {

    }
}*/

//pub fn panic_fmt(fmt: Arguments<'_>) -> !;

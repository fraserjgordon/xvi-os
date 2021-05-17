#![stable(feature="core_panic_info", since="1.41.0")]

#[lang = "panic_location"]
#[derive(Copy, Clone, Debug, Hash, PartialEq, PartialOrd, Eq, Ord)]
#[stable(feature="panic_hooks", since="1.10.0")]
pub struct Location<'a>
{
    file:   &'a str,
    line:   u32,
    column: u32,
}

#[lang = "panic_info"]
#[derive(Debug)]
#[stable(feature="panic_hooks", since="1.10.0")]
pub struct PanicInfo<'a>
{
    payload:    &'a (dyn Any + Send),
    message:    Option<&'a crate::fmt::Arguments<'a>>,
    location:   &'a Location<'a>,
}


impl <'a> Location<'a>
{
    #[stable(feature = "track_caller", since = "1.46.0")]
    #[rustc_const_unstable(feature = "const_caller_location", issue = "none")]
    #[track_caller]
    pub const fn caller() -> &'static Location<'static>
    {
        crate::intrinsics::caller_location()
    }
}


impl <'a> PanicInfo<'a>
{
    #[stable(feature="panic_hooks", since="1.10.0")]
    #[inline]
    pub fn payload(&self) -> &(dyn Any + Send)
    {
        self.payload
    }

    #[unstable(feature="panic_info_message", issue="none")]
    #[inline]
    pub fn message(&self) -> Option<&crate::fmt::Arguments<'_>>
    {
        self.message
    }

    #[stable(feature="panic_hooks", since="1.10.0")]
    #[inline]
    pub fn location(&self) -> Option<&Location<'_>>
    {
        Some(self.location)
    }
}

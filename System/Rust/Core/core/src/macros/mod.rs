#[macro_export]
#[rustc_builtin_macro]
macro_rules! cfg
{
    ($(_:tt)*) => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! column
{
    () => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! compile_error
{
    ($Msg:expr) => { /* builtin macro */ };
    ($Msg:expr,) => { /* builtin macro */ };
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! concat_idents
{
    ($(I:ident),+) => { /* builtin macro */ };
    ($(I:ident,)+) => { /* builtin macro */ };
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! file
{
    () => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! line
{
    () => { /* builtin macro */ }
}

#[macro_export]
macro_rules! panic
{
    () =>
        {
            panic!("panic (no message)")
        };

    ($Msg:literal) =>
        {
            crate::panicking::panic($Msg);
        };

    ($Msg:expr) =>
        {
            crate::panicking::panic("FIXME");
        };

    ($Msg:expr,) =>
        {
            crate::panicking::panic("FIXME");
        };

    ($Format:expr, $($Arg:tt)+) =>
        {
            crate::panicking::panic("FIXME");
        };
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! stringify
{
    ($(_:tt)*) => { /* builtin macro */ }
}

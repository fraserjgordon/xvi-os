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
macro_rules! const_format_args
{
    ($fmt:expr) => { /* builtin macro */ };
    ($fmt:expr, $($args:tt)*) => { /* builtin macro */ };
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! file
{
    () => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! format_args
{
    ($fmt:expr) => { /* builtin macro */ };
    ($fmt:expr, $($args:tt)*) => { /* builtin macro */ };
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! line
{
    () => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro(core_panic)]
macro_rules! panic
{
    ($($t:tt)+) => { /* builtin macro */ }
}

#[macro_export]
#[rustc_builtin_macro]
macro_rules! stringify
{
    ($(_:tt)*) => { /* builtin macro */ }
}


#[macro_export]
macro_rules! todo
{
    () => { panic!("TODO") };
    ($($_:tt)+) => { todo!() };
}

#[macro_export]
macro_rules! unimplemented
{
    () => { panic!("not implemented") };
    ($($_:tt)+) => { unimplemented!() };
}


pub mod builtin
{

#[rustc_builtin_macro]
pub macro derive($item:item)
{
    /* builtin macro */
}

}

use crate::marker::PhantomData;


mod integer;


pub(crate) mod macros
{

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics)]
pub macro Debug($_:item)
{
    /* builtin macro */
}
    
}


pub use macros::Debug;
use crate::mem;


pub type Result = crate::result::Result<(), Error>;


pub enum Alignment
{
    Left,
    Right,
    Center,
}


extern "C"
{
    type Opaque;
}

#[derive(Copy, Clone)]
pub struct Arguments<'a>
{
    // Contents are compiler-generated.
    pieces: &'a [&'static str],
    //fmt: Option<&'a [rt::v1::argument]>,
    fmt: Option<i32>,
    args: &'a [ArgumentV1<'a>],
}

#[derive(Copy, Clone)]
#[allow(missing_debug_implementations)]
pub struct ArgumentV1<'a>
{
    value: &'a Opaque,
    formatter: fn(&Opaque, &mut Formatter<'_>) -> Result,
}


pub trait Binary
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait Debug
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait Display
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait LowerExp
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait LowerHex
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait Octal
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait Pointer
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait UpperExp
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait UpperHex
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result;
}

pub trait Write
{
    fn write_str(&mut self, s: &str) -> Result;

    fn write_char(&mut self, c: char) -> Result;

    fn write_fmt(&mut self, args: Arguments<'_>) -> Result;
}


#[allow(missing_debug_implementations)]
pub struct DebugStruct<'a, 'b: 'a>
{
    formatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugTuple<'a, 'b: 'a>
{
    formatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugSet<'a, 'b: 'a>
{
    formatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugList<'a, 'b: 'a>
{
    formatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugMap<'a, 'b: 'a>
{
    formatter: &'a mut Formatter<'b>,
}

#[derive(Copy, Clone, Debug, Default, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct Error;

#[allow(missing_debug_implementations)]
pub struct Formatter<'a>
{
    buffer: &'a mut (dyn Write + 'a),
}


impl <'a> Arguments<'a>
{
    pub const unsafe fn new_v1(pieces: &'a [&'static str], args: &'a [ArgumentV1<'a>]) -> Arguments<'a>
    {
        Arguments
        {
            pieces: pieces,
            fmt: None,
            args: args,
        }
    }
}

impl <'a> ArgumentV1<'a>
{
    pub fn new<'b, T>(value: &'b T, formatter: fn(&T, &mut Formatter<'_>) -> Result) -> ArgumentV1<'b>
    {
        unsafe
        {
            ArgumentV1
            {
                value: mem::transmute(value),
                formatter: mem::transmute(formatter),
            }
        }
    }
}

impl <'a, 'b> DebugStruct<'a, 'b>
{
    pub fn field(&mut self, _name: &str, _value: &dyn Debug) -> &mut Self
    {
        self
    }
    
    pub fn finish(&mut self) -> Result
    {
        Ok(())
    }
}

impl <'a, 'b> DebugTuple<'a, 'b>
{
    pub fn field(&mut self, _value: &dyn Debug) -> &mut Self
    {
        self
    }

    pub fn finish(&mut self) -> Result
    {
        Ok(())
    }
}

impl <'a> Formatter<'a>
{
    pub fn pad_integral(&mut self, is_nonnegative: bool, prefix: &str, buf: &str) -> Result
    {
        todo!()
    }

    pub fn pad(&mut self, s: &str) -> Result
    {
        todo!()
    }

    pub fn write_str(&mut self, data: &str) -> Result
    {
        todo!()
    }

    pub fn write_fmt(&mut self, fmt: Arguments<'_>) -> Result
    {
        todo!()
    }

    pub fn fill(&self) -> char
    {
        todo!()
    }

    pub fn align(&self) -> Option<Alignment>
    {
        todo!()
    }

    pub fn width(&self) -> Option<usize>
    {
        todo!()
    }

    pub fn precision(&self) -> Option<usize>
    {
        todo!()
    }

    pub fn sign_plus(&self) -> bool
    {
        todo!()
    }

    pub fn sign_minus(&self) -> bool
    {
        todo!()
    }

    pub fn alternate(&self) -> bool
    {
        todo!()
    }

    pub fn sign_aware_zero_pad(&self) -> bool
    {
        todo!()
    }

    pub fn debug_struct<'b>(&'b mut self, _name: &str) -> DebugStruct<'b, 'a>
    {
        todo!()
    }    

    pub fn debug_tuple<'b>(&'b mut self, _name: &str) -> DebugTuple<'b, 'a>
    {
        todo!()
    }
}


impl <T: ?Sized + Debug> Debug for &T
{
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result
    {
        Debug::fmt(&**self, formatter)
    }
}

impl <T: ?Sized + Debug> Debug for &mut T
{
    fn fmt(&self, formatter: &mut Formatter<'_>) -> Result
    {
        Debug::fmt(&**self, formatter)
    }
}

impl <T: ?Sized> Debug for *const T
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl <T: ?Sized> Debug for *mut T
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Debug for !
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Debug for bool
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Debug for char
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Debug for str
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl <T: Debug> Debug for [T]
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Debug for ()
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl <T: ?Sized> Debug for PhantomData<T>
{
    fn fmt(&self, _formatter: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

/*impl <T: Copy + Debug> Debug for Cell<T>
{
    
}

impl <T: ?Sized + Debug> Debug for RefCell<T>
{

}*/

/*impl <T: ?Sized + Debug> Debug for Ref<'_, T>
{

}

impl <T: ?Sized + Debug> Debug for RefMut<'_, T>
{

}*/

/*impl <T: ?Sized + Debug> Debug for UnsafeCell<T>
{

}*/

impl Debug for Arguments<'_>
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result
    {
        todo!()
    }
}

impl Display for str
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result
    {
        f.pad(self)
    }
}

impl <T: ?Sized + Display> Display for &T
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result
    {
        Display::fmt(&**self, f)
    }
}

impl <T: ?Sized + Display> Display for &mut T
{
    fn fmt(&self, f: &mut Formatter<'_>) -> Result
    {
        Display::fmt(&**self, f)
    }
}

impl Write for Formatter<'_>
{
    fn write_str(&mut self, s: &str) -> Result
    {
        todo!()
    }

    fn write_char(&mut self, c: char) -> Result
    {
        todo!()
    }

    fn write_fmt(&mut self, args: Arguments<'_>) -> Result
    {
        todo!()
    }
}

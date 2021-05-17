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


pub type Result = crate::result::Result<(), Error>;


#[derive(Copy, Clone)]
pub struct Arguments<'a>
{
    // Contents are compiler-generated.
    pieces: &'a [&'static str],
    //fmt: Option<&'a [rt::v1::argument]>,
    //args: &'a [ArgumentV1<'a>],
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
    ormatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugList<'a, 'b: 'a>
{
    ormatter: &'a mut Formatter<'b>,
}

#[allow(missing_debug_implementations)]
pub struct DebugMap<'a, 'b: 'a>
{
    ormatter: &'a mut Formatter<'b>,
}

#[derive(Copy, Clone, Debug, Default, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct Error;

#[allow(missing_debug_implementations)]
pub struct Formatter<'a>
{
    buffer: &'a mut (dyn Write + 'a),
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

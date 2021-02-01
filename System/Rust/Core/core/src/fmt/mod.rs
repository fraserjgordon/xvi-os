pub(crate) mod macros
{

#[macro_export]
#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics)]
pub macro Debug($_:item)
{
    /* builtin macro */
}
    
}


type Result = crate::result::Result<(), Error>;


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

pub struct Error;

pub struct Formatter<'a>
{
    buffer: &'a mut (dyn Write + 'a),
}


impl <'a, 'b> DebugStruct<'a, 'b>
{
    pub fn field(&mut self, name: &str, value: &dyn Debug) -> &mut Self
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
    pub fn field(&mut self, value: &dyn Debug) -> &mut Self
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
    pub fn debug_struct<'b>(&'b mut self, name: &str) -> DebugStruct<'b, 'a>
    {

    }    

    pub fn debug_tuple<'b>(&'b mut self, name: &str) -> DebugTuple<'b, 'a>
    {

    }
}

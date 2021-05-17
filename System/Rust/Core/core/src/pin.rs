use crate::ops::{Deref, DerefMut};

#[lang = "pin"]
#[repr(transparent)]
pub struct Pin<P>
{
    ptr: P,
}


impl <P: Deref<Target: Unpin>> Pin<P>
{

}

impl <P: Deref> Pin<P>
{

}

impl <P: DerefMut> Pin<P>
{

}

impl <'a, T: ?Sized> Pin<&'a T>
{

}

impl <'a, T: ?Sized> Pin<&'a mut T>
{

}

impl <T: ?Sized> Pin<&'static T>
{

}

impl <T: ?Sized> Pin<&'static mut T>
{

}


impl <P: Deref> Deref for Pin<P>
{
    type Target = P::Target;

    fn deref(&self) -> &P::Target
    {
        todo!()
    }
}

impl <P: DerefMut<Target: Unpin>> DerefMut for Pin<P>
{
    fn deref_mut(&mut self) -> &mut P::Target
    {
        todo!()
    }
}

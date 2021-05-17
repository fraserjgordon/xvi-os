#![stable(feature="rust1", since="1.0.0")]


#[derive(Copy, Clone, Debug, Hash, PartialEq, PartialOrd, Eq, Ord)]
#[stable(feature="rust1", since="1.0.0")]
pub struct TypeId
{
    cookie: u64,
}


#[stable(feature="rust1", since="1.0.0")]
pub trait Any: 'static
{
    #[stable(feature="get_type_id", since="1.34.0")]
    fn type_id(&self) -> TypeId;
}


#[stable(feature="type_name", since="1.38.0")]
#[rustc_const_unstable(feature="const_type_name", issue="none")]
pub const fn type_name<T: ?Sized>() -> &'static str
{
    crate::intrinsics::type_name::<T>()
}

#[unstable(feature="type_name_of_val", issue="none")]
#[rustc_const_unstable(feature="const_type_name", issue="none")]
pub const fn type_name_of_val<T: ?Sized>(_: &T) -> &'static str
{
    type_name::<T>()
}


impl TypeId
{
    #[stable(feature="rust1", since="1.0.0")]
    pub fn of<T: ?Sized + 'static>() -> TypeId
    {
        TypeId { cookie: crate::intrinsics::type_id::<T>() }
    }
}


impl dyn Any
{
    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn is<T: Any>(&self) -> bool
    {
        self.type_id() == TypeId::of::<T>()
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_ref<T: Any>(&self) -> Option<&T>
    {
        if self.is::<T>()
        {
            unsafe
            {
                Some(&*(self as *const dyn Any as *const T))
            }
        }
        else
        {
            None
        }
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_mut<T: Any>(&mut self) -> Option<&mut T>
    {
        if self.is::<T>()
        {
            unsafe
            {
                Some(&mut *(self as *mut dyn Any as *mut T))
            }
        }
        else
        {
            None
        }
    }
}

impl dyn Any + Send
{
    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn is<T: Any>(&self) -> bool
    {
        <dyn Any>::is::<T>(self)
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_ref<T: Any>(&self) -> Option<&T>
    {
        <dyn Any>::downcast_ref::<T>(self)
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_mut<T: Any>(&mut self) -> Option<&mut T>
    {
        <dyn Any>::downcast_mut::<T>(self)
    }
}

impl dyn Any + Send + Sync
{
    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn is<T: Any>(&self) -> bool
    {
        <dyn Any>::is::<T>(self)
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_ref<T: Any>(&self) -> Option<&T>
    {
        <dyn Any>::downcast_ref::<T>(self)
    }

    #[stable(feature="rust1", since="1.0.0")]
    #[inline]
    pub fn downcast_mut<T: Any>(&mut self) -> Option<&mut T>
    {
        <dyn Any>::downcast_mut::<T>(self)
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl crate::fmt::Debug for dyn Any
{
    #[inline]
    fn fmt(&self, f: &mut crate::fmt::Formatter<'_>) -> crate::fmt::Result
    {
        todo!()
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl crate::fmt::Debug for dyn Any + Send
{
    #[inline]
    fn fmt(&self, f: &mut crate::fmt::Formatter<'_>) -> crate::fmt::Result
    {
        <dyn Any as crate::fmt::Debug>::fmt(self, f)
    }
}

#[stable(feature="any_send_sync_methods", since="1.28.0")]
impl crate::fmt::Debug for dyn Any + Send + Sync
{
    #[inline]
    fn fmt(&self, f: &mut crate::fmt::Formatter<'_>) -> crate::fmt::Result
    {
        <dyn Any as crate::fmt::Debug>::fmt(self, f)
    }
}


#[stable(feature="rust1", since="1.0.0")]
impl <T: 'static + ?Sized> Any for T
{
    fn type_id(&self) -> TypeId
    {
        TypeId::of::<T>()
    }
}

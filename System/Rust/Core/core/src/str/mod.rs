#![stable(feature="rust1", since="1.0.0")]


#[lang = "str"]
impl str
{

}


#[stable(feature="rust1", since="1.0.0")]
impl PartialEq<str> for str
{
    #[inline]
    fn eq(&self, other: &str) -> bool
    {
        todo!()
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl Eq for str {}

#[stable(feature="rust1", since="1.0.0")]
impl PartialOrd<str> for str
{
    #[inline]
    fn partial_cmp(&self, other: &str) -> Option<crate::cmp::Ordering>
    {
        todo!()
    }
}

#[stable(feature="rust1", since="1.0.0")]
impl Ord for str
{
    #[inline]
    fn cmp(&self, other: &str) -> crate::cmp::Ordering
    {
        todo!()
    }
}

pub trait Borrow<Borrowed: ?Sized>
{
    fn borrow(&self) -> &Borrowed;
}

pub trait BorrowMut<Borrowed: ?Sized>: Borrow<Borrowed>
{
    fn borrow_mut(&mut self) -> &mut Borrowed;
}

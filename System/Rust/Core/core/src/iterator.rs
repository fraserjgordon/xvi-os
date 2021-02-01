pub trait IntoIterator
{
    type Item;
    type IntoIter: Iterator<Item = Self::Item>;

    #[lang = "into_iter"]
    #[must_use]
    fn into_iter(self) -> Self::IntoIter;
}

#[must_use]
pub trait Iterator
{
    type Item;

    #[lang = "next"]
    fn next(&mut self) -> Option<Self::Item>;

    /*
    fn size_hint(&self) -> (usize, Option<usize>)
    {

    }

    fn count(self) -> usize
    where
        Self: Sized,
    {

    }

    fn last(self) -> Option<Self::Item>
    where
        Self: Sized,
    {

    }

    fn advance_by(&mut self, n: usize) -> Result<(), usize>
    {

    }

    fn nth(&mut self, n: usize) -> Option<Self::Item>
    {

    }

    fn step_by(self, step: usize) -> StepBy<Self>
    where
        Self: Sized,
    {

    }

    fn chain<U>(self, other: U) -> Chain<Self, U::IntoIter>
    where
        Self: Sized,
        U: IntoIterator<Item = Self::Item>,
    {

    }

    fn zip<U>(self, other: U) -> Zip<Self, U::IntoIter>
    where
        Self: Sized,
        U: IntoIterator<Item = Self::Item>,
    {

    }

    */
}

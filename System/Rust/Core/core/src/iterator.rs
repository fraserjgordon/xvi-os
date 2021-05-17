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

    
    #[inline]
    fn size_hint(&self) -> (usize, Option<usize>)
    {
        (0, None)
    }

    #[inline]
    fn count(self) -> usize
    where
        Self: Sized,
    {
        self.fold(0, |count: usize, _| count + 1)
    }

    #[inline]
    fn last(self) -> Option<Self::Item>
    where
        Self: Sized,
    {
        self.fold(None, |_, item| Some(item))
    }

    #[inline]
    fn advance_by(&mut self, n: usize) -> Result<(), usize>
    {
        let mut remaining = n;
        while remaining > 0
        {
            if let None = self.next()
            {
                return Err(n - remaining);
            }
            else
            {
                remaining -= 1;
            }
        }

        Ok(())
    }

    #[inline]
    fn nth(&mut self, n: usize) -> Option<Self::Item>
    {
        let mut remaining = n;
        let mut item: Option<Self::Item> = None;
        while remaining > 0
        {
            if let Some(x) = self.next()
            {
                item = Some(x);
            }
            else
            {
                return None;
            }

            remaining -= 1;
        }

        item
    }
/*
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

    #[inline]
    fn fold<B, F>(self, init: B, f: F) -> B
    where
        Self: Sized,
        F: FnMut(B, Self::Item) -> B
    {
        let mut iter = self;
        let mut accum = init;
        let mut func = f;

        while let Some(x) = iter.next()
        {
            accum = func(accum, x)
        }

        accum
    }
}

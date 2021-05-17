pub(crate) mod macros
{

#[rustc_builtin_macro]
#[allow_internal_unstable(core_intrinsics)]
pub macro Hash($_:item)
{
    /* builtin macro */
}

}


pub struct BuildHasherDefault<H>(crate::marker::PhantomData<H>);


pub trait BuildHasher
{
    type Hasher: Hasher;

    fn build_hasher(&self) -> Self::Hasher;
}

pub trait Hash
{
    fn hash<H: Hasher>(&self, state: &mut H);

    fn hash_slice<H: Hasher>(data: &[Self], state: &mut H)
    where
        Self: Sized
    {
        for d in data
        {
            d.hash(state)
        }
    }
}

pub trait Hasher
{
    fn finish(&self) -> u64;

    fn write(&mut self, bytes: &[u8]);

    fn write_u8(&mut self, i: u8);

    fn write_u16(&mut self, i: u16);

    fn write_u32(&mut self, i: u32);

    fn write_u64(&mut self, i: u64);

    fn write_u128(&mut self, i: u128);

    fn write_usize(&mut self, i: usize);

    fn write_i8(&mut self, i: i8);

    fn write_i16(&mut self, i: i16);

    fn write_i32(&mut self, i: i32);

    fn write_i64(&mut self, i: i64);

    fn write_i128(&mut self, i: i128);

    fn write_isize(&mut self, i: isize);
}


impl <H: Default + Hasher> BuildHasher for BuildHasherDefault<H>
{
    type Hasher = H;

    fn build_hasher(&self) -> H
    {
        H::default()
    }
}

impl <H: Hasher + ?Sized> Hasher for &mut H
{
    fn finish(&self) -> u64
    {
        (**self).finish()
    }

    fn write(&mut self, bytes: &[u8])
    {
        (**self).write(bytes)
    }

    fn write_u8(&mut self, i: u8)
    {
        (**self).write_u8(i)
    }

    fn write_u16(&mut self, i: u16)
    {
        (**self).write_u16(i)
    }

    fn write_u32(&mut self, i: u32)
    {
        (**self).write_u32(i)
    }

    fn write_u64(&mut self, i: u64)
    {
        (**self).write_u64(i)
    }

    fn write_u128(&mut self, i: u128)
    {
        (**self).write_u128(i)
    }

    fn write_usize(&mut self, i: usize)
    {
        (**self).write_usize(i)
    }

    fn write_i8(&mut self, i: i8)
    {
        (**self).write_i8(i)
    }

    fn write_i16(&mut self, i: i16)
    {
        (**self).write_i16(i)
    }

    fn write_i32(&mut self, i: i32)
    {
        (**self).write_i32(i)
    }

    fn write_i64(&mut self, i: i64)
    {
        (**self).write_i64(i)
    }

    fn write_i128(&mut self, i: i128)
    {
        (**self).write_i128(i)
    }

    fn write_isize(&mut self, i: isize)
    {
        (**self).write_isize(i)
    }
}


macro_rules! hash_impl
{
    ($T:ty, $Name:ident) =>
        (
            impl Hash for $T
            {
                fn hash<H: Hasher>(&self, state: &mut H)
                {
                    state.$Name(*self)
                }
            }
        )
}

hash_impl!{i8, write_i8}
hash_impl!{i16, write_i16}
hash_impl!{i32, write_i32}
hash_impl!{i64, write_i64}
hash_impl!{i128, write_i128}
hash_impl!{isize, write_isize}
hash_impl!{u8, write_u8}
hash_impl!{u16, write_u16}
hash_impl!{u32, write_u32}
hash_impl!{u64, write_u64}
hash_impl!{u128, write_u128}
hash_impl!{usize, write_usize}

impl Hash for !
{
    fn hash<H: Hasher>(&self, _state: &mut H) {}
}

impl Hash for ()
{
    fn hash<H: Hasher>(&self, state: &mut H) {}
}

impl Hash for bool
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        state.write_u8(*self as u8)
    }
}

impl Hash for char
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        state.write_u32(*self as u32)
    }
}

impl Hash for str
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        todo!()
    }
}

impl <T: Hash> Hash for [T]
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        Hash::hash_slice(self, state)
    }
}

impl <T: Hash + ?Sized> Hash for &T
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        (**self).hash(state)
    }
}

impl <T: Hash + ?Sized> Hash for &mut T
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        (**self).hash(state)
    }
}

impl <T: ?Sized> Hash for *const T
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        state.write_usize(*self as *const () as usize)
    }
}

impl <T: ?Sized> Hash for *mut T
{
    fn hash<H: Hasher>(&self, state: &mut H)
    {
        state.write_usize(*self as *const () as usize)
    }
}

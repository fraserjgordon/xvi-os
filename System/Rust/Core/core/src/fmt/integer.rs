macro_rules! impl_debug_for_integer
{
    ($t:ty) =>
    (
        impl super::Debug for $t
        {
            fn fmt(&self, f: &mut super::Formatter<'_>) -> super::Result
            {
                todo!()
            }
        }
    )
}

impl_debug_for_integer!{i8}
impl_debug_for_integer!{i16}
impl_debug_for_integer!{i32}
impl_debug_for_integer!{i64}
impl_debug_for_integer!{i128}

impl_debug_for_integer!{u8}
impl_debug_for_integer!{u16}
impl_debug_for_integer!{u32}
impl_debug_for_integer!{u64}
impl_debug_for_integer!{u128}

impl_debug_for_integer!{isize}
impl_debug_for_integer!{usize}

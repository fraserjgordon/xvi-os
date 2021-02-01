pub unsafe fn unreachable_unchecked() -> !
{
    unsafe
    {
        crate::intrinsics::unreachable()
    }
}

#[lang = "panic_location"]
pub struct Location<'a>
{
    file:   &'a str,
    line:   u32,
    col:    u32,
}

#[macro_use]
mod int_type;

int_type!{i8, 8, u8}
int_type!{i16, 16, u16}
int_type!{i32, 32, u32}
int_type!{i64, 64, u64}
int_type!{i128, 128, u128}

uint_type!{u8, 8}
uint_type!{u16, 16}
uint_type!{u32, 32}
uint_type!{u64, 64}
uint_type!{u128, 128}

const _PTR_BITS: u32 = 
    if      cfg!(target_pointer_width = "16") { 16 }
    else if cfg!(target_pointer_width = "32") { 32 }
    else if cfg!(target_pointer_width = "64") { 64 }
    else                                      { 0 };

int_type!{isize, _PTR_BITS, usize}
uint_type!{usize, _PTR_BITS}

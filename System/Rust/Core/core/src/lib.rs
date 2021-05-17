// As a core library implementation, this crate cannot depend on core.
#![no_core]
#![feature(no_core)]

#![feature(allow_internal_unstable)]
#![feature(lang_items)]
#![feature(rustc_attrs)]
#![feature(rustc_allow_const_fn_unstable)]
#![feature(staged_api)]

#![feature(associated_type_bounds)]
#![feature(auto_traits)]
#![feature(const_caller_location)]
#![feature(const_fn_union)]
#![feature(const_panic)]
#![feature(const_type_name)]
#![feature(decl_macro)]
#![feature(fundamental)]
#![feature(intrinsics)]
#![feature(negative_impls)]
#![feature(never_type)]
#![feature(prelude_import)]
#![feature(transparent_unions)]
#![feature(unboxed_closures)]
#![feature(untagged_unions)]

// REMOVE THESE
#![allow(dead_code)]
#![allow(unused_variables)]

#![unstable(feature="alpha", issue="none")]


// Primitive types.
/* #[path = "primitive_type/i8.rs"]
pub mod i8;

#[path = "primitive_type/i16.rs"]
pub mod i16;

#[path = "primitive_type/i32.rs"]
pub mod i32;

#[path = "primitive_type/i64.rs"]
pub mod i64;

#[path = "primitive_type/i128.rs"]
pub mod i128;

#[path = "primitive_type/u8.rs"]
pub mod u8;

#[path = "primitive_type/u16.rs"]
pub mod u16;

#[path = "primitive_type/u32.rs"]
pub mod u32;

#[path = "primitive_type/u64.rs"]
pub mod u64;

#[path = "primitive_type/u128.rs"]
pub mod u128; */

#[macro_use] pub mod macros;

#[prelude_import]
#[allow(unused)]
use prelude::v1::*;

pub mod any;
pub mod array;
pub mod borrow;
pub mod clone;
pub mod cmp;
pub mod convert;
pub mod default;
pub mod fmt;
pub mod hash;
pub mod hints;
pub mod intrinsics;
pub mod iterator;
pub mod marker;
pub mod mem;
pub mod num;
pub mod ops;
pub mod option;
pub mod panic;
pub mod panicking;
pub mod pin;
pub mod prelude;
//pub mod primitive;
pub mod ptr;
pub mod result;
pub mod slice;
pub mod str;

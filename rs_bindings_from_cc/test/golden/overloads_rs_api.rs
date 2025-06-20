// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:overloads_cc

#![rustfmt::skip]
#![feature(allocator_api, cfg_sanitize, custom_inner_attributes)]
#![allow(stable_features)]
#![no_std]
#![allow(improper_ctypes)]
#![allow(nonstandard_style)]
#![allow(dead_code, unused_mut)]
#![deny(warnings)]

// Error while generating bindings for item 'Overload':
// Cannot generate bindings for overloaded function

// Error while generating bindings for item 'Overload':
// Cannot generate bindings for overloaded function

/// Both Overload2() overloads should be generated, because one should be
/// renamed.
#[inline(always)]
pub fn Overload2() {
    unsafe { crate::detail::__rust_thunk___Z9Overload2v() }
}

#[inline(always)]
pub fn RenamedOverload2(__param_0: ::core::ffi::c_int) {
    unsafe { crate::detail::__rust_thunk___Z9Overlaod2i(__param_0) }
}

// Error while generating bindings for item 'UncallableOverload':
// Cannot generate bindings for overloaded function

// Error while generating bindings for item 'UncallableOverload':
// Cannot generate bindings for overloaded function

// Error while generating bindings for item 'Sizeof':
// Class templates are not supported yet

// Error while generating bindings for item 'UncallableOverload':
// Function templates are not supported yet

#[inline(always)]
pub fn AlsoTemplateOverload() {
    unsafe { crate::detail::__rust_thunk___Z20AlsoTemplateOverloadv() }
}

// Error while generating bindings for item 'AlsoTemplateOverload':
// Function templates are not supported yet

mod detail {
    #[allow(unused_imports)]
    use super::*;
    unsafe extern "C" {
        #[link_name = "_Z9Overload2v"]
        pub(crate) unsafe fn __rust_thunk___Z9Overload2v();
        #[link_name = "_Z9Overlaod2i"]
        pub(crate) unsafe fn __rust_thunk___Z9Overlaod2i(__param_0: ::core::ffi::c_int);
        pub(crate) unsafe fn __rust_thunk___Z20AlsoTemplateOverloadv();
    }
}

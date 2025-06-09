// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:unsupported_cc

#![rustfmt::skip]
#![feature(
    allocator_api,
    cfg_sanitize,
    custom_inner_attributes,
    impl_trait_in_assoc_type,
    negative_impls
)]
#![allow(stable_features)]
#![no_std]
#![allow(improper_ctypes)]
#![allow(nonstandard_style)]
#![allow(dead_code)]
#![deny(warnings)]

#[derive(Clone, Copy)]
#[repr(C)]
///CRUBIT_ANNOTATE: cpp_type=TrivialCustomType
pub struct TrivialCustomType {
    pub i: ::core::ffi::c_int,
}
impl !Send for TrivialCustomType {}
impl !Sync for TrivialCustomType {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("TrivialCustomType"),
    crate::TrivialCustomType
);

impl Default for TrivialCustomType {
    #[inline(always)]
    fn default() -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN17TrivialCustomTypeC1Ev(
                &raw mut tmp as *mut ::core::ffi::c_void,
            );
            tmp.assume_init()
        }
    }
}

impl<'b> From<::ctor::RvalueReference<'b, Self>> for TrivialCustomType {
    #[inline(always)]
    fn from(__param_0: ::ctor::RvalueReference<'b, Self>) -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN17TrivialCustomTypeC1EOS_(
                &raw mut tmp as *mut ::core::ffi::c_void,
                __param_0,
            );
            tmp.assume_init()
        }
    }
}
impl<'b> ::ctor::CtorNew<::ctor::RvalueReference<'b, Self>> for TrivialCustomType {
    type CtorType = Self;
    #[inline(always)]
    fn ctor_new(args: ::ctor::RvalueReference<'b, Self>) -> Self::CtorType {
        <Self as From<::ctor::RvalueReference<'b, Self>>>::from(args)
    }
}

impl<'b> ::ctor::UnpinAssign<&'b Self> for TrivialCustomType {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN17TrivialCustomTypeaSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::UnpinAssign<::ctor::RvalueReference<'b, Self>> for TrivialCustomType {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: ::ctor::RvalueReference<'b, Self>) {
        unsafe {
            crate::detail::__rust_thunk___ZN17TrivialCustomTypeaSEOS_(self, __param_0);
        }
    }
}

// Error while generating bindings for item 'TrivialCustomType::operator||':
// Bindings for this kind of operator (operator || with 2 parameter(s)) are not supported

// Error while generating bindings for item 'TrivialCustomType::operator int':
// Function name is not supported: Unsupported name: operator int

#[::ctor::recursively_pinned]
#[repr(C)]
///CRUBIT_ANNOTATE: cpp_type=NontrivialCustomType
pub struct NontrivialCustomType {
    __non_field_data: [::core::mem::MaybeUninit<u8>; 0],
    pub i: ::core::ffi::c_int,
}
impl !Send for NontrivialCustomType {}
impl !Sync for NontrivialCustomType {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("NontrivialCustomType"),
    crate::NontrivialCustomType
);

impl<'b> ::ctor::CtorNew<::ctor::RvalueReference<'b, Self>> for NontrivialCustomType {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: ::ctor::RvalueReference<'b, Self>) -> Self::CtorType {
        let __param_0 = args;
        unsafe {
            ::ctor::FnCtor::new(
                move |dest: ::core::pin::Pin<&mut ::core::mem::MaybeUninit<Self>>| {
                    crate::detail::__rust_thunk___ZN20NontrivialCustomTypeC1EOS_(
                        ::core::pin::Pin::into_inner_unchecked(dest) as *mut _
                            as *mut ::core::ffi::c_void,
                        __param_0,
                    );
                },
            )
        }
    }
}
impl<'b> ::ctor::CtorNew<(::ctor::RvalueReference<'b, Self>,)> for NontrivialCustomType {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: (::ctor::RvalueReference<'b, Self>,)) -> Self::CtorType {
        let (arg,) = args;
        <Self as ::ctor::CtorNew<::ctor::RvalueReference<'b, Self>>>::ctor_new(arg)
    }
}

// Error while generating bindings for item 'NontrivialCustomType::operator||':
// Bindings for this kind of operator (operator || with 2 parameter(s)) are not supported

// Error while generating bindings for item 'PackedLayout':
// Records with packed layout are not supported

// Error while generating bindings for item 'MultipleReasons':
// Parameter #0 is not supported: Unsupported type 'volatile int *': Unsupported `volatile` qualifier: volatile int
//
// Return type is not supported: Unsupported type 'volatile int *': Unsupported `volatile` qualifier: volatile int

#[derive(Clone, Copy)]
#[repr(C)]
///CRUBIT_ANNOTATE: cpp_type=ContainingStruct
pub struct ContainingStruct {
    /// Doc comment for an unsupported field.
    ///
    /// Reason for representing this field as a blob of bytes:
    /// Can't generate bindings for NestedStruct, because it is unsupported: b/200067824: type definitions nested inside records are not yet supported
    pub(crate) nested_struct: [::core::mem::MaybeUninit<u8>; 1],
}
impl !Send for ContainingStruct {}
impl !Sync for ContainingStruct {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("ContainingStruct"),
    crate::ContainingStruct
);

#[diagnostic::on_unimplemented(
    message = "binding generation for function failed\nUnsafe constructors (e.g. with no elided or explicit lifetimes) are intentionally not supported. See b/216648347."
)]
pub trait BindingFailedFor_ZN16ContainingStructC1Ev {}
impl<'error> Default for ContainingStruct
where
    &'error (): BindingFailedFor_ZN16ContainingStructC1Ev,
{
    #[inline(always)]
    fn default() -> Self {
        #![allow(unused_variables)]
        unreachable!(
            "This impl can never be instantiated. \
                    If this message appears at runtime, please report a <internal link>."
        )
    }
}

// Error while generating bindings for item 'ContainingStruct::ContainingStruct':
// Unsafe constructors (e.g. with no elided or explicit lifetimes) are intentionally not supported. See b/216648347.

#[diagnostic::on_unimplemented(
    message = "binding generation for function failed\nUnsafe constructors (e.g. with no elided or explicit lifetimes) are intentionally not supported. See b/216648347."
)]
pub trait BindingFailedFor_ZN16ContainingStructC1EOS_ {}
impl<'error, 'b> From<::ctor::RvalueReference<'b, Self>> for ContainingStruct
where
    &'error (): BindingFailedFor_ZN16ContainingStructC1EOS_,
{
    #[inline(always)]
    fn from(__param_0: ::ctor::RvalueReference<'b, Self>) -> Self {
        #![allow(unused_variables)]
        unreachable!(
            "This impl can never be instantiated. \
                    If this message appears at runtime, please report a <internal link>."
        )
    }
}

impl<'b> ::ctor::UnpinAssign<&'b Self> for ContainingStruct {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN16ContainingStructaSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::UnpinAssign<::ctor::RvalueReference<'b, Self>> for ContainingStruct {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: ::ctor::RvalueReference<'b, Self>) {
        unsafe {
            crate::detail::__rust_thunk___ZN16ContainingStructaSEOS_(self, __param_0);
        }
    }
}

// Error while generating bindings for item 'NestedStruct':
// Can't generate bindings for NestedStruct, because it is unsupported: b/200067824: type definitions nested inside records are not yet supported

mod detail {
    #[allow(unused_imports)]
    use super::*;
    unsafe extern "C" {
        pub(crate) unsafe fn __rust_thunk___ZN17TrivialCustomTypeC1Ev(
            __this: *mut ::core::ffi::c_void,
        );
        pub(crate) unsafe fn __rust_thunk___ZN17TrivialCustomTypeC1EOS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: ::ctor::RvalueReference<'b, crate::TrivialCustomType>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN17TrivialCustomTypeaSERKS_<'a, 'b>(
            __this: &'a mut crate::TrivialCustomType,
            __param_0: &'b crate::TrivialCustomType,
        ) -> &'a mut crate::TrivialCustomType;
        pub(crate) unsafe fn __rust_thunk___ZN17TrivialCustomTypeaSEOS_<'a, 'b>(
            __this: &'a mut crate::TrivialCustomType,
            __param_0: ::ctor::RvalueReference<'b, crate::TrivialCustomType>,
        ) -> &'a mut crate::TrivialCustomType;
        #[link_name = "_ZN20NontrivialCustomTypeC1EOS_"]
        pub(crate) unsafe fn __rust_thunk___ZN20NontrivialCustomTypeC1EOS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: ::ctor::RvalueReference<'b, crate::NontrivialCustomType>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN16ContainingStructaSERKS_<'a, 'b>(
            __this: &'a mut crate::ContainingStruct,
            __param_0: &'b crate::ContainingStruct,
        ) -> &'a mut crate::ContainingStruct;
        pub(crate) unsafe fn __rust_thunk___ZN16ContainingStructaSEOS_<'a, 'b>(
            __this: &'a mut crate::ContainingStruct,
            __param_0: ::ctor::RvalueReference<'b, crate::ContainingStruct>,
        ) -> &'a mut crate::ContainingStruct;
    }
}

const _: () = {
    assert!(::core::mem::size_of::<crate::TrivialCustomType>() == 4);
    assert!(::core::mem::align_of::<crate::TrivialCustomType>() == 4);
    static_assertions::assert_impl_all!(crate::TrivialCustomType: Copy,Clone);
    static_assertions::assert_not_impl_any!(crate::TrivialCustomType: Drop);
    assert!(::core::mem::offset_of!(crate::TrivialCustomType, i) == 0);
    assert!(::core::mem::size_of::<crate::NontrivialCustomType>() == 4);
    assert!(::core::mem::align_of::<crate::NontrivialCustomType>() == 4);
    static_assertions::assert_not_impl_any!(crate::NontrivialCustomType: Copy,Drop);
    assert!(::core::mem::offset_of!(crate::NontrivialCustomType, i) == 0);
    assert!(::core::mem::size_of::<crate::ContainingStruct>() == 1);
    assert!(::core::mem::align_of::<crate::ContainingStruct>() == 1);
    static_assertions::assert_impl_all!(crate::ContainingStruct: Copy,Clone);
    static_assertions::assert_not_impl_any!(crate::ContainingStruct: Drop);
    assert!(::core::mem::offset_of!(crate::ContainingStruct, nested_struct) == 0);
};

// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:polymorphic_cc

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
#![allow(dead_code, unused_mut)]
#![deny(warnings)]

#[::ctor::recursively_pinned(PinnedDrop)]
#[repr(C, align(8))]
///CRUBIT_ANNOTATE: cpp_type=PolymorphicBase
pub struct PolymorphicBase {
    __non_field_data: [::core::mem::MaybeUninit<u8>; 8],
}
impl !Send for PolymorphicBase {}
impl !Sync for PolymorphicBase {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("PolymorphicBase"),
    crate::PolymorphicBase
);

impl ::ctor::CtorNew<()> for PolymorphicBase {
    type CtorType = impl ::ctor::Ctor<Output = Self>;
    #[inline(always)]
    fn ctor_new(args: ()) -> Self::CtorType {
        let () = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN15PolymorphicBaseC1Ev(
                    dest as *mut ::core::ffi::c_void,
                );
            })
        }
    }
}

impl<'b> ::ctor::CtorNew<&'b Self> for PolymorphicBase {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: &'b Self) -> Self::CtorType {
        let mut __param_0 = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN15PolymorphicBaseC1ERKS_(
                    dest as *mut ::core::ffi::c_void,
                    __param_0,
                );
            })
        }
    }
}
impl<'b> ::ctor::CtorNew<(&'b Self,)> for PolymorphicBase {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: (&'b Self,)) -> Self::CtorType {
        let (arg,) = args;
        <Self as ::ctor::CtorNew<&'b Self>>::ctor_new(arg)
    }
}

impl<'b> ::ctor::Assign<&'b Self> for PolymorphicBase {
    #[inline(always)]
    fn assign<'a>(self: ::core::pin::Pin<&'a mut Self>, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN15PolymorphicBaseaSERKS_(self, __param_0);
        }
    }
}

impl ::ctor::PinnedDrop for PolymorphicBase {
    #[inline(always)]
    unsafe fn pinned_drop<'a>(self: ::core::pin::Pin<&'a mut Self>) {
        crate::detail::__rust_thunk___ZN15PolymorphicBaseD1Ev(self)
    }
}

#[::ctor::recursively_pinned(PinnedDrop)]
#[repr(C, align(8))]
///CRUBIT_ANNOTATE: cpp_type=PolymorphicBase2
pub struct PolymorphicBase2 {
    __non_field_data: [::core::mem::MaybeUninit<u8>; 8],
}
impl !Send for PolymorphicBase2 {}
impl !Sync for PolymorphicBase2 {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("PolymorphicBase2"),
    crate::PolymorphicBase2
);

impl ::ctor::CtorNew<()> for PolymorphicBase2 {
    type CtorType = impl ::ctor::Ctor<Output = Self>;
    #[inline(always)]
    fn ctor_new(args: ()) -> Self::CtorType {
        let () = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN16PolymorphicBase2C1Ev(
                    dest as *mut ::core::ffi::c_void,
                );
            })
        }
    }
}

impl<'b> ::ctor::CtorNew<&'b Self> for PolymorphicBase2 {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: &'b Self) -> Self::CtorType {
        let mut __param_0 = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN16PolymorphicBase2C1ERKS_(
                    dest as *mut ::core::ffi::c_void,
                    __param_0,
                );
            })
        }
    }
}
impl<'b> ::ctor::CtorNew<(&'b Self,)> for PolymorphicBase2 {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: (&'b Self,)) -> Self::CtorType {
        let (arg,) = args;
        <Self as ::ctor::CtorNew<&'b Self>>::ctor_new(arg)
    }
}

impl<'b> ::ctor::Assign<&'b Self> for PolymorphicBase2 {
    #[inline(always)]
    fn assign<'a>(self: ::core::pin::Pin<&'a mut Self>, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN16PolymorphicBase2aSERKS_(self, __param_0);
        }
    }
}

impl PolymorphicBase2 {
    #[inline(always)]
    pub fn Foo<'a>(self: ::core::pin::Pin<&'a mut Self>) {
        unsafe { crate::detail::__rust_thunk___ZN16PolymorphicBase23FooEv(self) }
    }
}

impl ::ctor::PinnedDrop for PolymorphicBase2 {
    #[inline(always)]
    unsafe fn pinned_drop<'a>(self: ::core::pin::Pin<&'a mut Self>) {
        crate::detail::__rust_thunk___ZN16PolymorphicBase2D1Ev(self)
    }
}

#[::ctor::recursively_pinned(PinnedDrop)]
#[repr(C, align(8))]
///CRUBIT_ANNOTATE: cpp_type=PolymorphicDerived
pub struct PolymorphicDerived {
    __non_field_data: [::core::mem::MaybeUninit<u8>; 16],
}
impl !Send for PolymorphicDerived {}
impl !Sync for PolymorphicDerived {}
forward_declare::unsafe_define!(
    forward_declare::symbol!("PolymorphicDerived"),
    crate::PolymorphicDerived
);

impl ::ctor::CtorNew<()> for PolymorphicDerived {
    type CtorType = impl ::ctor::Ctor<Output = Self>;
    #[inline(always)]
    fn ctor_new(args: ()) -> Self::CtorType {
        let () = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN18PolymorphicDerivedC1Ev(
                    dest as *mut ::core::ffi::c_void,
                );
            })
        }
    }
}

impl<'b> ::ctor::CtorNew<&'b Self> for PolymorphicDerived {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: &'b Self) -> Self::CtorType {
        let mut __param_0 = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN18PolymorphicDerivedC1ERKS_(
                    dest as *mut ::core::ffi::c_void,
                    __param_0,
                );
            })
        }
    }
}
impl<'b> ::ctor::CtorNew<(&'b Self,)> for PolymorphicDerived {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: (&'b Self,)) -> Self::CtorType {
        let (arg,) = args;
        <Self as ::ctor::CtorNew<&'b Self>>::ctor_new(arg)
    }
}

impl<'b> ::ctor::CtorNew<::ctor::RvalueReference<'b, Self>> for PolymorphicDerived {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: ::ctor::RvalueReference<'b, Self>) -> Self::CtorType {
        let mut __param_0 = args;
        unsafe {
            ::ctor::FnCtor::new(move |dest: *mut Self| {
                crate::detail::__rust_thunk___ZN18PolymorphicDerivedC1EOS_(
                    dest as *mut ::core::ffi::c_void,
                    __param_0,
                );
            })
        }
    }
}
impl<'b> ::ctor::CtorNew<(::ctor::RvalueReference<'b, Self>,)> for PolymorphicDerived {
    type CtorType = impl ::ctor::Ctor<Output = Self> + use<'b>;
    #[inline(always)]
    fn ctor_new(args: (::ctor::RvalueReference<'b, Self>,)) -> Self::CtorType {
        let (arg,) = args;
        <Self as ::ctor::CtorNew<::ctor::RvalueReference<'b, Self>>>::ctor_new(arg)
    }
}

impl ::ctor::PinnedDrop for PolymorphicDerived {
    #[inline(always)]
    unsafe fn pinned_drop<'a>(self: ::core::pin::Pin<&'a mut Self>) {
        crate::detail::__rust_thunk___ZN18PolymorphicDerivedD1Ev(self)
    }
}

impl<'b> ::ctor::Assign<&'b Self> for PolymorphicDerived {
    #[inline(always)]
    fn assign<'a>(self: ::core::pin::Pin<&'a mut Self>, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN18PolymorphicDerivedaSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::Assign<::ctor::RvalueReference<'b, Self>> for PolymorphicDerived {
    #[inline(always)]
    fn assign<'a>(
        self: ::core::pin::Pin<&'a mut Self>,
        __param_0: ::ctor::RvalueReference<'b, Self>,
    ) {
        unsafe {
            crate::detail::__rust_thunk___ZN18PolymorphicDerivedaSEOS_(self, __param_0);
        }
    }
}

mod detail {
    #[allow(unused_imports)]
    use super::*;
    unsafe extern "C" {
        pub(crate) unsafe fn __rust_thunk___ZN15PolymorphicBaseC1Ev(
            __this: *mut ::core::ffi::c_void,
        );
        pub(crate) unsafe fn __rust_thunk___ZN15PolymorphicBaseC1ERKS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: &'b crate::PolymorphicBase,
        );
        pub(crate) unsafe fn __rust_thunk___ZN15PolymorphicBaseaSERKS_<'a, 'b>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicBase>,
            __param_0: &'b crate::PolymorphicBase,
        ) -> ::core::pin::Pin<&'a mut crate::PolymorphicBase>;
        pub(crate) unsafe fn __rust_thunk___ZN15PolymorphicBaseD1Ev<'a>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicBase>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN16PolymorphicBase2C1Ev(
            __this: *mut ::core::ffi::c_void,
        );
        pub(crate) unsafe fn __rust_thunk___ZN16PolymorphicBase2C1ERKS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: &'b crate::PolymorphicBase2,
        );
        pub(crate) unsafe fn __rust_thunk___ZN16PolymorphicBase2aSERKS_<'a, 'b>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicBase2>,
            __param_0: &'b crate::PolymorphicBase2,
        ) -> ::core::pin::Pin<&'a mut crate::PolymorphicBase2>;
        pub(crate) unsafe fn __rust_thunk___ZN16PolymorphicBase23FooEv<'a>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicBase2>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN16PolymorphicBase2D1Ev<'a>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicBase2>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedC1Ev(
            __this: *mut ::core::ffi::c_void,
        );
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedC1ERKS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: &'b crate::PolymorphicDerived,
        );
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedC1EOS_<'b>(
            __this: *mut ::core::ffi::c_void,
            __param_0: ::ctor::RvalueReference<'b, crate::PolymorphicDerived>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedD1Ev<'a>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicDerived>,
        );
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedaSERKS_<'a, 'b>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicDerived>,
            __param_0: &'b crate::PolymorphicDerived,
        ) -> ::core::pin::Pin<&'a mut crate::PolymorphicDerived>;
        pub(crate) unsafe fn __rust_thunk___ZN18PolymorphicDerivedaSEOS_<'a, 'b>(
            __this: ::core::pin::Pin<&'a mut crate::PolymorphicDerived>,
            __param_0: ::ctor::RvalueReference<'b, crate::PolymorphicDerived>,
        ) -> ::core::pin::Pin<&'a mut crate::PolymorphicDerived>;
    }
}

const _: () = {
    assert!(::core::mem::size_of::<crate::PolymorphicBase>() == 8);
    assert!(::core::mem::align_of::<crate::PolymorphicBase>() == 8);
    static_assertions::assert_impl_all!(crate::PolymorphicBase: Drop);
    static_assertions::assert_not_impl_any!(crate::PolymorphicBase: Copy);

    assert!(::core::mem::size_of::<crate::PolymorphicBase2>() == 8);
    assert!(::core::mem::align_of::<crate::PolymorphicBase2>() == 8);
    static_assertions::assert_impl_all!(crate::PolymorphicBase2: Drop);
    static_assertions::assert_not_impl_any!(crate::PolymorphicBase2: Copy);

    assert!(::core::mem::size_of::<crate::PolymorphicDerived>() == 16);
    assert!(::core::mem::align_of::<crate::PolymorphicDerived>() == 8);
    static_assertions::assert_impl_all!(crate::PolymorphicDerived: Drop);
    static_assertions::assert_not_impl_any!(crate::PolymorphicDerived: Copy);
};

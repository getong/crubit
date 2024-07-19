// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:comment_cc
// Features: experimental, supported

#![rustfmt::skip]
#![feature(custom_inner_attributes, negative_impls, register_tool)]
#![allow(stable_features)]
#![no_std]
#![register_tool(__crubit)]
#![allow(improper_ctypes)]
#![allow(nonstandard_style)]
#![deny(warnings)]

// File comment

// TODO(b/202933018): Re-enable once namespaces are supported
// namespace ns {
// a

/// Foo
#[derive(Clone, Copy)]
#[repr(C)]
#[__crubit::annotate(cpp_type = "Foo")]
pub struct Foo {
    /// A field
    pub i: ::core::ffi::c_int,
    /// Another field
    pub j: ::core::ffi::c_int,
}
impl !Send for Foo {}
impl !Sync for Foo {}
forward_declare::unsafe_define!(forward_declare::symbol!("Foo"), crate::Foo);

impl Default for Foo {
    #[inline(always)]
    fn default() -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN3FooC1Ev(&mut tmp);
            tmp.assume_init()
        }
    }
}

impl<'b> From<::ctor::RvalueReference<'b, Self>> for Foo {
    #[inline(always)]
    fn from(__param_0: ::ctor::RvalueReference<'b, Self>) -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN3FooC1EOS_(&mut tmp, __param_0);
            tmp.assume_init()
        }
    }
}

impl<'b> ::ctor::UnpinAssign<&'b Self> for Foo {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN3FooaSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::UnpinAssign<::ctor::RvalueReference<'b, Self>> for Foo {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: ::ctor::RvalueReference<'b, Self>) {
        unsafe {
            crate::detail::__rust_thunk___ZN3FooaSEOS_(self, __param_0);
        }
    }
}

// TODO(rosica): This comment appears near fields of a struct, and
// is currently generated below the struct definiton on the Rust side.

// TODO(rosica): This comment appears between fields of a struct, and
// is currently generated below the struct definiton on the Rust side.

// TODO(rosica): This comment appears near fields of a struct, and
// is currently generated below the struct definiton on the Rust side.

// b

// }  // namespace ns

// c

/// foo
#[inline(always)]
pub fn foo() {
    unsafe { crate::detail::__rust_thunk___Z3foov() }
}

/// Bar
#[derive(Clone, Copy)]
#[repr(C)]
#[__crubit::annotate(cpp_type = "Bar")]
pub struct Bar {
    pub i: ::core::ffi::c_int,
}
impl !Send for Bar {}
impl !Sync for Bar {}
forward_declare::unsafe_define!(forward_declare::symbol!("Bar"), crate::Bar);

impl Default for Bar {
    #[inline(always)]
    fn default() -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN3BarC1Ev(&mut tmp);
            tmp.assume_init()
        }
    }
}

impl<'b> From<::ctor::RvalueReference<'b, Self>> for Bar {
    #[inline(always)]
    fn from(__param_0: ::ctor::RvalueReference<'b, Self>) -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN3BarC1EOS_(&mut tmp, __param_0);
            tmp.assume_init()
        }
    }
}

impl<'b> ::ctor::UnpinAssign<&'b Self> for Bar {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN3BaraSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::UnpinAssign<::ctor::RvalueReference<'b, Self>> for Bar {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: ::ctor::RvalueReference<'b, Self>) {
        unsafe {
            crate::detail::__rust_thunk___ZN3BaraSEOS_(self, __param_0);
        }
    }
}

/// d
#[derive(Clone, Copy)]
#[repr(C)]
#[__crubit::annotate(cpp_type = "HasNoComments")]
pub struct HasNoComments {
    pub i: ::core::ffi::c_int,
}
impl !Send for HasNoComments {}
impl !Sync for HasNoComments {}
forward_declare::unsafe_define!(forward_declare::symbol!("HasNoComments"), crate::HasNoComments);

impl Default for HasNoComments {
    #[inline(always)]
    fn default() -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN13HasNoCommentsC1Ev(&mut tmp);
            tmp.assume_init()
        }
    }
}

impl<'b> From<::ctor::RvalueReference<'b, Self>> for HasNoComments {
    #[inline(always)]
    fn from(__param_0: ::ctor::RvalueReference<'b, Self>) -> Self {
        let mut tmp = ::core::mem::MaybeUninit::<Self>::zeroed();
        unsafe {
            crate::detail::__rust_thunk___ZN13HasNoCommentsC1EOS_(&mut tmp, __param_0);
            tmp.assume_init()
        }
    }
}

impl<'b> ::ctor::UnpinAssign<&'b Self> for HasNoComments {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: &'b Self) {
        unsafe {
            crate::detail::__rust_thunk___ZN13HasNoCommentsaSERKS_(self, __param_0);
        }
    }
}

impl<'b> ::ctor::UnpinAssign<::ctor::RvalueReference<'b, Self>> for HasNoComments {
    #[inline(always)]
    fn unpin_assign<'a>(&'a mut self, __param_0: ::ctor::RvalueReference<'b, Self>) {
        unsafe {
            crate::detail::__rust_thunk___ZN13HasNoCommentsaSEOS_(self, __param_0);
        }
    }
}

// e

mod detail {
    #[allow(unused_imports)]
    use super::*;
    extern "C" {
        pub(crate) fn __rust_thunk___ZN3FooC1Ev<'a>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::Foo>,
        );
        pub(crate) fn __rust_thunk___ZN3FooC1EOS_<'a, 'b>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::Foo>,
            __param_0: ::ctor::RvalueReference<'b, crate::Foo>,
        );
        pub(crate) fn __rust_thunk___ZN3FooaSERKS_<'a, 'b>(
            __this: &'a mut crate::Foo,
            __param_0: &'b crate::Foo,
        ) -> &'a mut crate::Foo;
        pub(crate) fn __rust_thunk___ZN3FooaSEOS_<'a, 'b>(
            __this: &'a mut crate::Foo,
            __param_0: ::ctor::RvalueReference<'b, crate::Foo>,
        ) -> &'a mut crate::Foo;
        pub(crate) fn __rust_thunk___Z3foov();
        pub(crate) fn __rust_thunk___ZN3BarC1Ev<'a>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::Bar>,
        );
        pub(crate) fn __rust_thunk___ZN3BarC1EOS_<'a, 'b>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::Bar>,
            __param_0: ::ctor::RvalueReference<'b, crate::Bar>,
        );
        pub(crate) fn __rust_thunk___ZN3BaraSERKS_<'a, 'b>(
            __this: &'a mut crate::Bar,
            __param_0: &'b crate::Bar,
        ) -> &'a mut crate::Bar;
        pub(crate) fn __rust_thunk___ZN3BaraSEOS_<'a, 'b>(
            __this: &'a mut crate::Bar,
            __param_0: ::ctor::RvalueReference<'b, crate::Bar>,
        ) -> &'a mut crate::Bar;
        pub(crate) fn __rust_thunk___ZN13HasNoCommentsC1Ev<'a>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::HasNoComments>,
        );
        pub(crate) fn __rust_thunk___ZN13HasNoCommentsC1EOS_<'a, 'b>(
            __this: &'a mut ::core::mem::MaybeUninit<crate::HasNoComments>,
            __param_0: ::ctor::RvalueReference<'b, crate::HasNoComments>,
        );
        pub(crate) fn __rust_thunk___ZN13HasNoCommentsaSERKS_<'a, 'b>(
            __this: &'a mut crate::HasNoComments,
            __param_0: &'b crate::HasNoComments,
        ) -> &'a mut crate::HasNoComments;
        pub(crate) fn __rust_thunk___ZN13HasNoCommentsaSEOS_<'a, 'b>(
            __this: &'a mut crate::HasNoComments,
            __param_0: ::ctor::RvalueReference<'b, crate::HasNoComments>,
        ) -> &'a mut crate::HasNoComments;
    }
}

const _: () = {
    assert!(::core::mem::size_of::<crate::Foo>() == 8);
    assert!(::core::mem::align_of::<crate::Foo>() == 4);
    static_assertions::assert_impl_all!(crate::Foo: Clone);
    static_assertions::assert_impl_all!(crate::Foo: Copy);
    static_assertions::assert_not_impl_any!(crate::Foo: Drop);
    assert!(::core::mem::offset_of!(crate::Foo, i) == 0);
    assert!(::core::mem::offset_of!(crate::Foo, j) == 4);

    assert!(::core::mem::size_of::<crate::Bar>() == 4);
    assert!(::core::mem::align_of::<crate::Bar>() == 4);
    static_assertions::assert_impl_all!(crate::Bar: Clone);
    static_assertions::assert_impl_all!(crate::Bar: Copy);
    static_assertions::assert_not_impl_any!(crate::Bar: Drop);
    assert!(::core::mem::offset_of!(crate::Bar, i) == 0);

    assert!(::core::mem::size_of::<crate::HasNoComments>() == 4);
    assert!(::core::mem::align_of::<crate::HasNoComments>() == 4);
    static_assertions::assert_impl_all!(crate::HasNoComments: Clone);
    static_assertions::assert_impl_all!(crate::HasNoComments: Copy);
    static_assertions::assert_not_impl_any!(crate::HasNoComments: Drop);
    assert!(::core::mem::offset_of!(crate::HasNoComments, i) == 0);
};

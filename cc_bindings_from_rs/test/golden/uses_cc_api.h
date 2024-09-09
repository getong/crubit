// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated C++ bindings for the following Rust crate:
// uses_rust
// Features: experimental, supported

// clang-format off
#pragma once

#include "support/internal/attribute_macros.h"
#include "support/internal/return_value_slot.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace uses_rust {

namespace test_use_glob {

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=8
std::int32_t f1();

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=12
std::int32_t f2();

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=20
struct CRUBIT_INTERNAL_RUST_TYPE(":: uses_rust :: test_use_glob :: X1") alignas(
    4) [[clang::trivial_abi]] X1 final {
 public:
  // `test_use_glob::X1` doesn't implement the `Default` trait
  X1() = delete;

  // No custom `Drop` impl and no custom \"drop glue\" required
  ~X1() = default;
  X1(X1&&) = default;
  X1& operator=(X1&&) = default;

  // `test_use_glob::X1` doesn't implement the `Clone` trait
  X1(const X1&) = delete;
  X1& operator=(const X1&) = delete;

 private:
  union {
    // Generated from:
    // cc_bindings_from_rs/test/golden/uses.rs;l=21
    std::int32_t x;
  };

 private:
  static void __crubit_field_offset_assertions();
};

}  // namespace test_use_glob

using ::uses_rust::test_use_glob::f1;
using ::uses_rust::test_use_glob::f2;
using X1 = ::uses_rust::test_use_glob::X1;

namespace __crubit_internal {

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=32
struct CRUBIT_INTERNAL_RUST_TYPE(":: uses_rust :: Bar") alignas(4)
    [[clang::trivial_abi]] Bar final {
 public:
  // `private_module::Bar` doesn't implement the `Default` trait
  Bar() = delete;

  // No custom `Drop` impl and no custom \"drop glue\" required
  ~Bar() = default;
  Bar(Bar&&) = default;
  Bar& operator=(Bar&&) = default;

  // `private_module::Bar` doesn't implement the `Clone` trait
  Bar(const Bar&) = delete;
  Bar& operator=(const Bar&) = delete;

 private:
  union {
    // Generated from:
    // cc_bindings_from_rs/test/golden/uses.rs;l=33
    std::int32_t i;
  };

 private:
  static void __crubit_field_offset_assertions();
};

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=35
struct CRUBIT_INTERNAL_RUST_TYPE(":: uses_rust :: Foo") alignas(4)
    [[clang::trivial_abi]] Foo final {
 public:
  // `private_module::Foo` doesn't implement the `Default` trait
  Foo() = delete;

  // No custom `Drop` impl and no custom \"drop glue\" required
  ~Foo() = default;
  Foo(Foo&&) = default;
  Foo& operator=(Foo&&) = default;

  // `private_module::Foo` doesn't implement the `Clone` trait
  Foo(const Foo&) = delete;
  Foo& operator=(const Foo&) = delete;

  // Generated from:
  // cc_bindings_from_rs/test/golden/uses.rs;l=41
  static ::uses_rust::__crubit_internal::Foo create();

  // Generated from:
  // cc_bindings_from_rs/test/golden/uses.rs;l=45
  static ::uses_rust::__crubit_internal::Bar bar();

 private:
  union {
    // Generated from:
    // cc_bindings_from_rs/test/golden/uses.rs;l=36
    std::int32_t i;
  };

 public:
  union {
    // Generated from:
    // cc_bindings_from_rs/test/golden/uses.rs;l=37
    ::uses_rust::__crubit_internal::Bar bar_;
  };

 private:
  static void __crubit_field_offset_assertions();
};

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=54
std::int32_t g1();

// Generated from:
// cc_bindings_from_rs/test/golden/uses.rs;l=58
std::int32_t g2();

}  // namespace __crubit_internal

using Bar = ::uses_rust::__crubit_internal::Bar;
using Foo = ::uses_rust::__crubit_internal::Foo;
using ::uses_rust::__crubit_internal::g1;
using ::uses_rust::__crubit_internal::g2;

namespace test_use_glob {

namespace __crubit_internal {
extern "C" std::int32_t __crubit_thunk_f1();
}
inline std::int32_t f1() { return __crubit_internal::__crubit_thunk_f1(); }

namespace __crubit_internal {
extern "C" std::int32_t __crubit_thunk_f2();
}
inline std::int32_t f2() { return __crubit_internal::__crubit_thunk_f2(); }

static_assert(
    sizeof(X1) == 4,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(
    alignof(X1) == 4,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(std::is_trivially_destructible_v<X1>);
static_assert(std::is_trivially_move_constructible_v<X1>);
static_assert(std::is_trivially_move_assignable_v<X1>);
inline void X1::__crubit_field_offset_assertions() {
  static_assert(0 == offsetof(X1, x));
}
}  // namespace test_use_glob

namespace __crubit_internal {

static_assert(
    sizeof(Bar) == 4,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(
    alignof(Bar) == 4,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(std::is_trivially_destructible_v<Bar>);
static_assert(std::is_trivially_move_constructible_v<Bar>);
static_assert(std::is_trivially_move_assignable_v<Bar>);
inline void Bar::__crubit_field_offset_assertions() {
  static_assert(0 == offsetof(Bar, i));
}
static_assert(
    sizeof(Foo) == 8,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(
    alignof(Foo) == 4,
    "Verify that ADT layout didn't change since this header got generated");
static_assert(std::is_trivially_destructible_v<Foo>);
static_assert(std::is_trivially_move_constructible_v<Foo>);
static_assert(std::is_trivially_move_assignable_v<Foo>);
namespace __crubit_internal {
extern "C" void __crubit_thunk_create(
    ::uses_rust::__crubit_internal::Foo* __ret_ptr);
}
inline ::uses_rust::__crubit_internal::Foo Foo::create() {
  crubit::ReturnValueSlot<::uses_rust::__crubit_internal::Foo> __ret_slot;
  __crubit_internal::__crubit_thunk_create(__ret_slot.Get());
  return std::move(__ret_slot).AssumeInitAndTakeValue();
}

namespace __crubit_internal {
extern "C" void __crubit_thunk_bar(
    ::uses_rust::__crubit_internal::Bar* __ret_ptr);
}
inline ::uses_rust::__crubit_internal::Bar Foo::bar() {
  crubit::ReturnValueSlot<::uses_rust::__crubit_internal::Bar> __ret_slot;
  __crubit_internal::__crubit_thunk_bar(__ret_slot.Get());
  return std::move(__ret_slot).AssumeInitAndTakeValue();
}
inline void Foo::__crubit_field_offset_assertions() {
  static_assert(0 == offsetof(Foo, i));
  static_assert(4 == offsetof(Foo, bar_));
}
namespace __crubit_internal {
extern "C" std::int32_t __crubit_thunk_g1();
}
inline std::int32_t g1() { return __crubit_internal::__crubit_thunk_g1(); }

namespace __crubit_internal {
extern "C" std::int32_t __crubit_thunk_g2();
}
inline std::int32_t g2() { return __crubit_internal::__crubit_thunk_g2(); }

}  // namespace __crubit_internal

}  // namespace uses_rust

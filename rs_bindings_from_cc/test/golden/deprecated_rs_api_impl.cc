// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:deprecated_cc

#include "support/internal/cxx20_backports.h"
#include "support/internal/offsetof.h"
#include "support/internal/sizeof.h"

#include <cstddef>
#include <memory>

// Public headers of the C++ library being wrapped.
#include "rs_bindings_from_cc/test/golden/deprecated.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wthread-safety-analysis"

static_assert(CRUBIT_SIZEOF(struct DeprecatedFields) == 8);
static_assert(alignof(struct DeprecatedFields) == 4);
static_assert(CRUBIT_OFFSET_OF(no_message, struct DeprecatedFields) == 0);
static_assert(CRUBIT_OFFSET_OF(message, struct DeprecatedFields) == 4);

extern "C" void __rust_thunk___ZN16DeprecatedFieldsC1Ev(
    struct DeprecatedFields* __this) {
  crubit::construct_at(__this);
}

#pragma clang diagnostic pop

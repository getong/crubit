// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Automatically @generated Rust bindings for the following C++ target:
// //rs_bindings_from_cc/test/golden:non_member_operator_cc

#include "support/internal/cxx20_backports.h"
#include "support/internal/offsetof.h"
#include "support/internal/sizeof.h"

#include <cstddef>
#include <memory>

// Public headers of the C++ library being wrapped.
#include "rs_bindings_from_cc/test/golden/non_member_operator.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wthread-safety-analysis"

static_assert(CRUBIT_SIZEOF(struct ns::X) == 4);
static_assert(alignof(struct ns::X) == 4);
static_assert(CRUBIT_OFFSET_OF(f, struct ns::X) == 0);

extern "C" void __rust_thunk___ZN2ns1XC1Ev(struct ns::X* __this) {
  crubit::construct_at(__this);
}

extern "C" void __rust_thunk___ZN2ns1XC1EOS0_(struct ns::X* __this,
                                              struct ns::X* __param_0) {
  crubit::construct_at(__this, std::move(*__param_0));
}

extern "C" struct ns::X* __rust_thunk___ZN2ns1XaSERKS0_(
    struct ns::X* __this, const struct ns::X* __param_0) {
  return &__this->operator=(*__param_0);
}

extern "C" struct ns::X* __rust_thunk___ZN2ns1XaSEOS0_(
    struct ns::X* __this, struct ns::X* __param_0) {
  return &__this->operator=(std::move(*__param_0));
}

extern "C" bool __rust_thunk___ZeqN2ns1XES0_(struct ns::X* a, struct ns::X* b) {
  return operator==(std::move(*a), std::move(*b));
}

#pragma clang diagnostic pop

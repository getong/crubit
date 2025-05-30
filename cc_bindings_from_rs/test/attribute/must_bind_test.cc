// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "cc_bindings_from_rs/test/attribute/must_bind.h"

#include "gtest/gtest.h"

namespace {

TEST(MustBindTest, GetsBound) {
  must_bind::Original::new_();
  must_bind::bar();
}

}  // namespace

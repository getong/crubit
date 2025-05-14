// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "cc_bindings_from_rs/test/never/never.h"

#include "gtest/gtest.h"

namespace {

TEST(NeverTest, NeverReturnDoesNotReturn) {
  EXPECT_DEATH(never::never_return(), "You can't do that!");
}

TEST(NeverTest, ExternNeverReturnDoesNotReturn) {
  EXPECT_DEATH(never::extern_never_return(), "You can't do that directly!");
}

TEST(NeverTest, AssocatedFnNeverReturnDoesNotReturn) {
  EXPECT_DEATH(never::NeverStruct::associated_fn_never_return(),
               "You can't do that as an associated fn!");
}

TEST(NeverTest, MethodNeverReturnDoesNotReturn) {
  never::NeverStruct never_struct;
  EXPECT_DEATH(never_struct.method_never_return(),
               "You can't do that as a method!");
}

}  // namespace

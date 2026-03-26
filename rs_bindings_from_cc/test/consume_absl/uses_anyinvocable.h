// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef THIRD_PARTY_CRUBIT_RS_BINDINGS_FROM_CC_TEST_CONSUME_ABSL_USES_ANYINVOCABLE_H_
#define THIRD_PARTY_CRUBIT_RS_BINDINGS_FROM_CC_TEST_CONSUME_ABSL_USES_ANYINVOCABLE_H_

#include <optional>

#include "absl/functional/any_invocable.h"

// Calls the invocable and returns void.
void CallVoidVoid(absl::AnyInvocable<void() &&> f);

// Returns an invocable that increments its argument.
absl::AnyInvocable<int(int) const> ReturnIntVoid();

absl::AnyInvocable<std::optional<int>(std::optional<int>) const>
ReturnOptionalIntMapper();

#endif  // THIRD_PARTY_CRUBIT_RS_BINDINGS_FROM_CC_TEST_CONSUME_ABSL_USES_ANYINVOCABLE_H_

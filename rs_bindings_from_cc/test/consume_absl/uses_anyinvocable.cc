// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "rs_bindings_from_cc/test/consume_absl/uses_anyinvocable.h"

#include <optional>
#include <utility>

#include "absl/functional/any_invocable.h"

void CallVoidVoid(absl::AnyInvocable<void() &&> f) { std::move(f)(); }

absl::AnyInvocable<int(int) const> ReturnIntVoid() {
  return [](int x) -> int { return x + 1; };
}

absl::AnyInvocable<std::optional<int>(std::optional<int>) const>
ReturnOptionalIntMapper() {
  return [](std::optional<int> x) -> std::optional<int> {
    if (x.has_value()) {
      return x.value() + 1;
    }
    return std::nullopt;
  };
}

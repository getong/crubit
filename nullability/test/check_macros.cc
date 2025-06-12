// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests for CHECK_NE and some of its implementation details.

#include <memory>

#include "check.h"
#include "nullability_test.h"

TEST void getReferenceableValueModeledForAbseil(int *_Nullable NullablePtr,
                                                int *_Nonnull NonnullPtr) {
  auto &Nullable2 = absl::something::GetReferenceableValue(NullablePtr);
  auto &Nonnull2 = absl::something::GetReferenceableValue(NonnullPtr);
  // Outputs should share the nullability of inputs.
  nullable(Nullable2);
  nonnull(Nonnull2);
}

TEST void getReferenceableValueModeledForUtil(int *_Nullable NullablePtr,
                                              int *_Nonnull NonnullPtr) {
  auto &Nullable2 = util::something::GetReferenceableValue(NullablePtr);
  auto &Nonnull2 = util::something::GetReferenceableValue(NonnullPtr);
  // Outputs should share the nullability of inputs.
  nullable(Nullable2);
  nonnull(Nonnull2);
}

TEST void abslCheckNEImplModelEqualAndNull() {
  int *P = nullptr;
  // `P` is definitely equal to `nullptr`, so result is nonnull.
  nonnull(absl::something::Check_NEImpl(P, nullptr, "foo"));
}

TEST void abslCheckNEImplModelMaybeUnequal(int *P, int *Q) {
  // Since we don't know `P`'s relation to  `Q`, result may be null.
  nullable(absl::something::Check_NEImpl(P, Q, "foo"));
}

TEST void abslCheckNEImPlModelUnequal(int *P) {
  if (P != nullptr) {
    // `P` is definitely not equal to `nullptr`, so result is null.
    nullable(absl::something::Check_NEImpl(P, nullptr, "foo"));
  }
}

TEST void checkNELeft(int *_Nullable P) {
  CHECK_NE(P, nullptr);
  nonnull(P);
}

TEST void checkNERight(int *_Nullable P) {
  CHECK_NE(nullptr, P);
  nonnull(P);
}

TEST void checkNELeftSmartPointer(std::unique_ptr<int> P) {
  CHECK_NE(P, nullptr);
  nonnull(P);
}

TEST void checkNERightSmartPointer(std::unique_ptr<int> P) {
  CHECK_NE(nullptr, P);
  nonnull(P);
}

TEST void utilCheckNEImplModelEqualAndNull() {
  int *P = nullptr;
  // `P` is definitely equal to `nullptr`, so result is nonnull.
  nonnull(util::something::Check_NEImpl(P, nullptr, "foo"));
}

TEST void utilCheckNEImplModelMaybeUnequal(int *P, int *Q) {
  // Since we don't know `P`'s relation to  `Q`, result may be null.
  nullable(util::something::Check_NEImpl(P, Q, "foo"));
}

TEST void utilCheckNEImPlModelUnequal(int *P) {
  if (P != nullptr) {
    // `P` is definitely not equal to `nullptr`, so result is null.
    nullable(util::something::Check_NEImpl(P, nullptr, "foo"));
  }
}

// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests for pointer arithmetic.

#include "nullability/test/check_diagnostics.h"
#include "external/llvm-project/third-party/unittest/googletest/include/gtest/gtest.h"

namespace clang::tidy::nullability {
namespace {

TEST(PointerNullabilityTest, PointerArithmeticNullable) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target(int *_Nullable nullable, int i) {
      int *orig = nullable;

      // Operations without side-effects.

      // On a nullable pointer, the pointer arithmetic itself should already be
      // considered unsafe, unless we know that the offset is zero.
      nullable + i;  // [[unsafe]]
      nullable - i;  // [[unsafe]]
      nullable + 0;
      nullable - 0;

      // Unary `+` is safe on a nullable pointer.
      +nullable;

      // Operations with side-effects; these need to restore the original value
      // of `nullable` every time.

      nullable++;  // [[unsafe]]
      nullable = orig;

      ++nullable;  // [[unsafe]]
      nullable = orig;

      nullable--;  // [[unsafe]]
      nullable = orig;

      --nullable;  // [[unsafe]]
      nullable = orig;

      nullable += 1;  // [[unsafe]]
      nullable = orig;

      nullable -= 1;  // [[unsafe]]
      nullable = orig;
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerArithmeticNonnull) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target(int *_Nonnull nonnull, int i) {
      int *orig = nonnull;

      nonnull + i;
      nonnull - i;
      +nonnull;

      ++nonnull;
      nonnull = orig;

      nonnull++;
      nonnull = orig;

      --nonnull;
      nonnull = orig;

      nonnull--;
      nonnull = orig;

      nonnull += 1;
      nonnull = orig;

      nonnull -= 1;
      nonnull = orig;
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerArithmeticUnknown) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target(int *unknown, int i) {
      int *orig = unknown;

      unknown + i;
      unknown - i;
      +unknown;

      ++unknown;
      unknown = orig;

      unknown++;
      unknown = orig;

      --unknown;
      unknown = orig;

      unknown--;
      unknown = orig;

      unknown += 1;
      unknown = orig;

      unknown -= 1;
      unknown = orig;
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerDifference) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target(int *_Nullable nullable, int *_Nonnull nonnull, int *unknown) {
      nullable - nullable;  // [[unsafe]]
      nullable - nonnull;   // [[unsafe]]
      nullable - unknown;   // [[unsafe]]

      nonnull - nullable;  // [[unsafe]]
      nonnull - nonnull;
      nonnull - unknown;

      unknown - nullable;  // [[unsafe]]
      unknown - nonnull;
      unknown - unknown;
    }
  )cc"));
}

}  // namespace
}  // namespace clang::tidy::nullability

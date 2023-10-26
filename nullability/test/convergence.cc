// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests that analysis converges (but not prematurely).
// Many of these tests involve pointer variables that are mutated in a loop
// because this is a common source of non-convergence.

#include "nullability/test/check_diagnostics.h"
#include "third_party/llvm/llvm-project/third-party/unittest/googletest/include/gtest/gtest.h"

namespace clang::tidy::nullability {
namespace {

// The following tests are variations on a theme: We loop over pointers where
// the first and subsequent pointers are returned by different functions with
// potentially different nullability. If either of the functions returns a
// nullable pointer, we should warn about the dereference.

TEST(PointerNullabilityTest, PointerLoop_Nullable_Nullable) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetFirst();
    int* _Nullable GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;  // [[unsafe]]
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Nonnull_Nullable) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nonnull GetFirst();
    int* _Nullable GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        // TODO: False negative. This dereference should be flagged as unsafe.
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Nullable_Nonnull) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetFirst();
    int* _Nonnull GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;  // [[unsafe]]
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Nonnull_Nonnull) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nonnull GetFirst();
    int* _Nonnull GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Unknown_Nullable) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* GetFirst();
    int* _Nullable GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        // TODO: False negative. This dereference should be flagged as unsafe.
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Unknown_Nonnull) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* GetFirst();
    int* _Nonnull GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Nullable_Unknown) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetFirst();
    int* GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;  // [[unsafe]]
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Nonnull_Unknown) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nonnull GetFirst();
    int* GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, PointerLoop_Unknown_Unknown) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* GetFirst();
    int* GetNext();
    void target() {
      for (int* p = GetFirst();; p = GetNext()) {
        *p;
      }
    }
  )cc"));
}

// If we check that the pointer is non-null, don't warn.
TEST(PointerNullabilityTest, PointerLoop_Checked) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetFirst();
    int* _Nullable GetNext();
    void target() {
      for (int* p = GetFirst(); p != nullptr; p = GetNext()) {
        *p;
      }
    }
  )cc"));
}

// If there is a loop condition but it is unrelated to the pointer value, warn.
// Only `GetNext()` returns `_Nullable` to test that the check analyzes more
// than just the first iteration.
TEST(PointerNullabilityTest, PointerLoop_UnrelatedCondition) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nonnull GetFirst();
    int* _Nullable GetNext();
    bool cond();
    void target() {
      for (int* p = GetFirst(); cond(); p = GetNext()) {
        // TODO: False negative. This dereference should be flagged as unsafe.
        *p;
      }
    }
  )cc"));
}

// Similar to `PointerLoop_UnrelatedCondition`, but we use a counted loop.
TEST(PointerNullabilityTest, PointerLoop_Counted) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nonnull GetFirst();
    int* _Nullable GetNext();
    void target() {
      int* p = GetFirst();
      for (int i = 0; i < 10; ++i, p = GetNext()) {
        // TODO: False negative. This dereference should be flagged as unsafe.
        *p;
      }
    }
  )cc"));
}

// Various tests for convergence of range-based for loops.

TEST(PointerNullabilityTest, RangeFor_Array_ByValue) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target() {
      int array[10];
      for (int i : array)
        ;
    }
  )cc"));
}

TEST(PointerNullabilityTest, RangeFor_Array_ByReference) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    void target() {
      int array[10];
      for (const int& i : array)
        ;
    }
  )cc"));
}

TEST(PointerNullabilityTest, RangeFor_CustomContainer) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Container {
      int* begin();
      int* end();
    };

    void target() {
      Container container;
      for (const int& i : container)
        ;
    }
  )cc"));
}

TEST(PointerNullabilityTest, RangeFor_TemplatedContainer) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    struct Container {
      T* begin();
      T* end();
    };

    void target() {
      Container<int> container;
      for (const int& i : container)
        ;
    }
  )cc"));
}

TEST(PointerNullabilityTest, RangeFor_CustomIterator) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Iterator {
      bool operator==(const Iterator& I) const;
      bool operator!=(const Iterator& I) const;
      int& operator*() const;
      Iterator operator++();
    };
    struct Container {
      Iterator begin();
      Iterator end();
    };

    void target() {
      Container container;
      for (const int& i : container)
        ;
    }
  )cc"));
}

// Currently fails (produces a false positive), because of a framework bug. We
// need an extra narrowing step to fix this.
TEST(PointerNullabilityTest, WhileAssignment) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetNext();
    void target() {
      int* p;
      while ((p = GetNext())) {
        *p;
      }
    }
  )cc"));
}

TEST(PointerNullabilityTest, WhileAssignment2) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* GetFirst();
    int* _Nullable GetNext();
    void target() {
      int* _Nullable p = GetFirst();
      while ((p = GetNext()) != nullptr) {
        *p;
      }
    }
  )cc"));
}

// Repro for a false positive caused by an inconsistent representation of state
// in a loop.
TEST(PointerNullabilityTest, InconsistentLoopStateRepro) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    int* _Nullable GetNullable();
    bool cond();

    void target(int* b, int* e) {
      // This loop is necessary for the false positive to occur.
      for (; b != e; ++b)
        ;

      int* ptr = GetNullable();
      if (ptr != nullptr) {
        while (cond()) {
          // TODO: False positive. This dereference is safe.
          (void)*ptr;  // [[unsafe]]
        }
      }
    }
  )cc"));
}

}  // namespace
}  // namespace clang::tidy::nullability
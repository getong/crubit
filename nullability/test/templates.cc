// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Tests for nullability annotations in template arguments.

#include "nullability/test/check_diagnostics.h"
#include "external/llvm-project/third-party/unittest/googletest/include/gtest/gtest.h"

namespace clang::tidy::nullability {
namespace {

TEST(PointerNullabilityTest, ClassTemplateInstantiation) {
  // Class template specialization with one argument initialised as _Nullable.
  // We test types that contain both nullability that is substituted into the
  // template argument and nullability that is spelt inside the template. That
  // is, we should be able to accurately store nullabilities from different
  // sources in a single nullability vector.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0>
    struct Struct1Arg {
      T0 arg0;
      T0 *unknownTPtr;
      T0 *_Nullable nullableTPtr;
      T0 *_Nonnull nonnullTPtr;

      T0 getT();
      T0 *getUnknownTPtr();
      T0 *_Nullable getNullableTPtr();
      T0 *_Nonnull getNonnullTPtr();
    };
    void target(Struct1Arg<int *_Nullable> p) {
      *p.arg0;  // [[unsafe]]
      *p.unknownTPtr;
      *p.nullableTPtr;  // [[unsafe]]
      *p.nonnullTPtr;
      **p.unknownTPtr;   // [[unsafe]]
      **p.nullableTPtr;  // [[unsafe]]
      **p.nonnullTPtr;   // [[unsafe]]

      *p.getT();  // [[unsafe]]
      *p.getUnknownTPtr();
      *p.getNullableTPtr();  // [[unsafe]]
      *p.getNonnullTPtr();
      **p.getUnknownTPtr();   // [[unsafe]]
      **p.getNullableTPtr();  // [[unsafe]]
      **p.getNonnullTPtr();   // [[unsafe]]
    }
  )cc"));

  // Class template specialization with one argument initialised as _Nonnull.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0>
    struct Struct1Arg {
      T0 arg0;
      T0 *unknownTPtr;
      T0 *_Nullable nullableTPtr;
      T0 *_Nonnull nonnullTPtr;

      T0 getT();
      T0 *getUnknownTPtr();
      T0 *_Nullable getNullableTPtr();
      T0 *_Nonnull getNonnullTPtr();
    };

    void target(Struct1Arg<int *_Nonnull> p) {
      *p.getT();
      *p.getUnknownTPtr();
      *p.getNullableTPtr();  // [[unsafe]]
      *p.getNonnullTPtr();
      **p.getUnknownTPtr();
      **p.getNullableTPtr();  // [[unsafe]]
      **p.getNonnullTPtr();

      *p.arg0;
      *p.unknownTPtr;
      *p.nullableTPtr;  // [[unsafe]]
      *p.nonnullTPtr;
      **p.unknownTPtr;
      **p.nullableTPtr;  // [[unsafe]]
      **p.nonnullTPtr;
    }
  )cc"));

  // Class template specialization with one argument initialised without
  // nullability annotation.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0>
    struct Struct1Arg {
      T0 arg0;
      T0 *unknownTPtr;
      T0 *_Nullable nullableTPtr;
      T0 *_Nonnull nonnullTPtr;

      T0 getT();
      T0 *getUnknownTPtr();
      T0 *_Nullable getNullableTPtr();
      T0 *_Nonnull getNonnullTPtr();
    };

    void target(Struct1Arg<int *> p) {
      *p.getT();
      *p.getUnknownTPtr();
      *p.getNullableTPtr();  // [[unsafe]]
      *p.getNonnullTPtr();
      **p.getUnknownTPtr();
      **p.getNullableTPtr();  // [[unasfe]]
      **p.getNonnullTPtr();

      *p.arg0;
      *p.unknownTPtr;
      *p.nullableTPtr;  // [[unsafe]]
      *p.nonnullTPtr;
      **p.unknownTPtr;
      **p.nullableTPtr;  // [[unsafe]]
      **p.nonnullTPtr;
    }
  )cc"));

  // Class template specialization with two arguments, whose second argument is
  // initialized as nullable.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    struct Struct2Arg {
      T0 arg0;
      T0 *unknownT0Ptr;
      T0 *_Nullable nullableT0Ptr;
      T0 *_Nonnull nonnullT0Ptr;

      T1 arg1;
      T1 *unknownT1Ptr;
      T1 *_Nullable nullableT1Ptr;
      T1 *_Nonnull nonnullT1Ptr;

      T0 getT0();
      T0 *getUnknownT0Ptr();
      T0 *_Nullable getNullableT0Ptr();
      T0 *_Nonnull getNonnullT0Ptr();

      T1 getT1();
      T1 *getUnknownT1Ptr();
      T1 *_Nullable getNullableT1Ptr();
      T1 *_Nonnull getNonnullT1Ptr();
    };

    void target(Struct2Arg<int *_Nonnull, double *_Nullable> p) {
      *p.arg0;
      *p.arg1;  // [[unsafe]]

      *p.unknownT0Ptr;
      *p.nullableT0Ptr;  // [[unsafe]]
      *p.nonnullT0Ptr;

      *p.unknownT1Ptr;
      *p.nullableT1Ptr;  // [[unsafe]]
      *p.nonnullT1Ptr;

      *p.getUnknownT0Ptr();
      *p.getNullableT0Ptr();  // [[unsafe]]
      *p.getNonnullT0Ptr();

      *p.getUnknownT1Ptr();
      *p.getNullableT1Ptr();  // [[unsafe]]
      *p.getNonnullT1Ptr();
    }
  )cc"));

  // Class template specialization with 5 arguments with interleaved
  // nullable/nonnull/unknown.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1, typename T2, typename T3, typename T4>
    struct Struct5Arg {
      T0 arg0;
      T1 arg1;
      T2 arg2;
      T3 arg3;
      T4 arg4;

      T0 getT0();
      T1 getT1();
      T2 getT2();
      T3 getT3();
      T4 getT4();
    };
    void target(Struct5Arg<int *_Nullable, double *_Nonnull, float *,
                           double *_Nullable, int *_Nonnull>
                    p) {
      *p.arg0;  // [[unsafe]]
      *p.arg1;
      *p.arg2;
      *p.arg3;  // [[unsafe]]
      *p.arg4;

      *p.getT0();  // [[unsafe]]
      *p.getT1();
      *p.getT2();
      *p.getT3();  // [[unsafe]]
      *p.getT4();
    }
  )cc"));

  // Class template specialization with 5 arguments with interleaved
  // nullable/nonnull/unknown/const.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1, typename T2, typename T3, typename T4>
    struct Struct5Arg {
      T0 arg0;
      T1 arg1;
      T2 arg2;
      T3 arg3;
      T4 arg4;

      T0 getT0();
      T1 getT1();
      T2 getT2();
      T3 getT3();
      T4 getT4();
    };
    void target(
        Struct5Arg<int *const _Nullable, double const *const _Nonnull, float *,
                   double const *const _Nullable, int *_Nonnull>
            p) {
      *p.arg0;  // [[unsafe]]
      *p.arg1;
      *p.arg2;
      *p.arg3;  // [[unsafe]]
      *p.arg4;

      *p.getT0();  // [[unsafe]]
      *p.getT1();
      *p.getT2();
      *p.getT3();  // [[unsafe]]
      *p.getT4();
    }
  )cc"));

  // Class template specialization with interleaved int and type template
  // parameters.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <int I0, typename T1, int I2, typename T3, int I4, typename T5>
    struct Struct6ArgWithInt {
      T1 arg1;
      T3 arg3;
      T5 arg5;

      T1 getT1();
      T3 getT3();
      T5 getT5();
    };
    void target(
        Struct6ArgWithInt<0, int *_Nullable, 1, int *_Nullable, 2, int *> &x) {
      *x.arg1;  // [[unsafe]]
      *x.arg3;  // [[unsafe]]
      *x.arg5;

      *x.getT1();  // [[unsafe]]
      *x.getT3();  // [[unsafe]]
      *x.getT5();
    }
  )cc"));
}

TEST(PointerNullabilityTest,
     ClassTemplateInstantiationWithStructsAsParameters) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct3IntPtrs {
      int *unknown;
      int *_Nullable nullable;
      int *_Nonnull nonnull;

      int *getUnknown();
      int *_Nullable getNullable();
      int *_Nonnull getNonnull();
    };

    template <typename T0>
    struct Struct1Arg {
      T0 arg0;
      T0 getT0();
    };

    void target(Struct1Arg<Struct3IntPtrs> p) {
      *p.arg0.unknown;
      *p.arg0.nullable;  // [[unsafe]]
      *p.arg0.nonnull;

      *p.arg0.getUnknown();
      *p.arg0.getNullable();  // [[unsafe]]
      *p.arg0.getNonnull();

      *p.getT0().unknown;
      *p.getT0().nullable;  // [[unsafe]]
      *p.getT0().nonnull;

      *p.getT0().getUnknown();
      *p.getT0().getNullable();  // [[unsafe]]
      *p.getT0().getNonnull();
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct1UnknownArg {
      char *unknownChar;

      char *getUnknownChar();
    };

    struct Struct1NullableArg {
      char *_Nullable nullableChar;

      char *_Nullable getNullableChar();
    };

    struct Struct1NonnullArg {
      char *_Nonnull nonnullChar;

      char *_Nonnull getNonnullChar();
    };

    struct StructLotsOfArgs {
      int num;
      long long *unknownLongLong;
      double *_Nullable nullableDouble;
      float *_Nonnull nonnullFloat;
      short *unknownShort;
      unsigned int *_Nullable nullableUInt;
      bool *_Nullable nullableBool;

      long long *getUnknownLongLong();
      double *_Nullable getNullableDouble();
      float *_Nonnull getNonnullFloat();
      short *getUnknownShort();
      unsigned int *_Nullable getNullableUInt();
      bool *_Nullable getNullableBool();
    };

    template <typename T0, typename T1, typename T2, typename T3>
    struct Struct4Arg {
      T0 arg0;
      T1 arg1;
      T2 arg2;
      T3 arg3;

      T0 getT0();
      T1 getT1();
      T2 getT2();
      T3 getT3();
    };

    void target(Struct4Arg<Struct1UnknownArg, Struct1NullableArg,
                           Struct1NonnullArg, StructLotsOfArgs>
                    p) {
      *p.arg0.unknownChar;
      *p.arg1.nullableChar;  // [[unsafe]]
      *p.arg2.nonnullChar;
      *p.arg3.unknownLongLong;
      *p.arg3.nullableDouble;  // [[unsafe]]
      *p.arg3.nonnullFloat;
      *p.arg3.unknownShort;
      *p.arg3.nullableUInt;  // [[unsafe]]
      *p.arg3.nullableBool;  // [[unsafe]]

      *p.arg0.getUnknownChar();
      *p.arg1.getNullableChar();  // [[unsafe]]
      *p.arg2.getNonnullChar();
      *p.arg3.getUnknownLongLong();
      *p.arg3.getNullableDouble();  // [[unsafe]]
      *p.arg3.getNonnullFloat();
      *p.arg3.getUnknownShort();
      *p.arg3.getNullableUInt();  // [[unsafe]]
      *p.arg3.getNullableBool();  // [[unsafe]]

      *p.getT0().unknownChar;
      *p.getT1().nullableChar;  // [[unsafe]]
      *p.getT2().nonnullChar;
      *p.getT3().unknownLongLong;
      *p.getT3().nullableDouble;  // [[unsafe]]
      *p.getT3().nonnullFloat;
      *p.getT3().unknownShort;
      *p.getT3().nullableUInt;  // [[unsafe]]
      *p.getT3().nullableBool;  // [[unsafe]]

      *p.getT0().getUnknownChar();
      *p.getT1().getNullableChar();  // [[unsafe]]
      *p.getT2().getNonnullChar();
      *p.getT3().getUnknownLongLong();
      *p.getT3().getNullableDouble();  // [[unsafe]]
      *p.getT3().getNonnullFloat();
      *p.getT3().getUnknownShort();
      *p.getT3().getNullableUInt();  // [[unsafe]]
      *p.getT3().getNullableBool();  // [[unsafe]]
    }
  )cc"));

  // With const arguments and int template parameter.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct1UnknownArg {
      char *const constUnknownChar;
      char const *unknownConstChar;
      char const *const constUnknownConstChar;

      char *const getConstUnknownChar();
      char const *getUnknownConstChar();
      char const *const getConstUnknownConstChar();
    };

    struct Struct1NullableArg {
      char *const _Nullable constNullableChar;
      char const *_Nullable nullableConstChar;
      char const *const _Nullable constNullableConstChar;

      char *const _Nullable getConstNullableChar();
      char const *_Nullable getNullableConstChar();
      char *const *_Nullable getConstNullableConstChar();
    };

    struct Struct1NonnullArg {
      char *const _Nonnull constNonnullChar;
      char const *_Nonnull nonnullConstChar;
      char const *const _Nonnull constNonnullConstChar;

      char *const _Nonnull getConstNonnullChar();
      char const *_Nonnull getNonnullConstChar();
      char const *const _Nonnull getConstNonnullConstChar();
    };

    template <int I0, typename T1, typename T2, typename T3>
    struct Struct4Arg {
      T1 arg1;
      T2 arg2;
      T3 arg3;

      T1 getT1();
      T2 getT2();
      T3 getT3();
    };

    void target(
        Struct4Arg<4, Struct1UnknownArg, Struct1NullableArg, Struct1NonnullArg>
            p) {
      *p.arg1.constUnknownChar;
      *p.arg1.unknownConstChar;
      *p.arg1.constUnknownConstChar;
      *p.arg2.constNullableChar;       // [[unsafe]]
      *p.arg2.nullableConstChar;       // [[unsafe]]
      *p.arg2.constNullableConstChar;  // [[unsafe]]
      *p.arg3.constNonnullChar;
      *p.arg3.nonnullConstChar;
      *p.arg3.constNonnullConstChar;

      *p.arg1.getConstUnknownChar();
      *p.arg1.getUnknownConstChar();
      *p.arg1.getConstUnknownConstChar();
      *p.arg2.getConstNullableChar();       // [[unsafe]]
      *p.arg2.getNullableConstChar();       // [[unsafe]]
      *p.arg2.getConstNullableConstChar();  // [[unsafe]]
      *p.arg3.getConstNonnullChar();
      *p.arg3.getNonnullConstChar();
      *p.arg3.getConstNonnullConstChar();

      *p.getT1().constUnknownChar;
      *p.getT1().unknownConstChar;
      *p.getT1().constUnknownConstChar;
      *p.getT2().constNullableChar;       // [[unsafe]]
      *p.getT2().nullableConstChar;       // [[unsafe]]
      *p.getT2().constNullableConstChar;  // [[unsafe]]
      *p.getT3().constNonnullChar;
      *p.getT3().nonnullConstChar;
      *p.getT3().constNonnullConstChar;

      *p.getT1().getConstUnknownChar();
      *p.getT1().getUnknownConstChar();
      *p.getT1().getConstUnknownConstChar();
      *p.getT2().getConstNullableChar();       // [[unsafe]]
      *p.getT2().getNullableConstChar();       // [[unsafe]]
      *p.getT2().getConstNullableConstChar();  // [[unsafe]]
      *p.getT3().getConstNonnullChar();
      *p.getT3().getNonnullConstChar();
      *p.getT3().getConstNonnullConstChar();
    }
  )cc"));
}

TEST(PointerNullabilityTest, MemberFunctionTemplateOfConcreteStruct) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct S {
      template <typename T0>
      T0 getT0();
    };

    void target(S p) {
      *p.getT0<int *>();
      *p.getT0<int *_Nonnull>();
      *p.getT0<int *_Nullable>();  // [[unsafe]]

      *p.getT0<int const *>();
      *p.getT0<int *const>();
      *p.getT0<int const *const>();
      *p.getT0<int const *_Nonnull>();
      *p.getT0<int *const _Nonnull>();
      *p.getT0<int const *const _Nonnull>();
      *p.getT0<int const *_Nullable>();        // [[unsafe]]
      *p.getT0<int *const _Nullable>();        // [[unsafe]]
      *p.getT0<int const *const _Nullable>();  // [[unsafe]]
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct S {
      template <int I0, typename T1, int I2>
      T1 getT1();
    };

    void target(S p) {
      *p.getT1<0, int *, 1>();
      *p.getT1<2147483647, int *_Nonnull, -2147483647>();
      *p.getT1<4, int *_Nullable, 4>();  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, MemberFunctionTemplateOfTemplateStruct) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0>
    struct S {
      template <typename TN1>
      TN1 getTN1();
    };

    void target(S<int> p) {
      *p.getTN1<int *>();
      *p.getTN1<int *_Nonnull>();
      *p.getTN1<int *_Nullable>();  // [[unsafe]]

      *p.getTN1<int const *>();
      *p.getTN1<int *const>();
      *p.getTN1<int const *const>();
      *p.getTN1<int const *_Nonnull>();
      *p.getTN1<int *const _Nonnull>();
      *p.getTN1<int const *const _Nonnull>();
      *p.getTN1<int const *_Nullable>();        // [[unsafe]]
      *p.getTN1<int *const _Nullable>();        // [[unsafe]]
      *p.getTN1<int const *const _Nullable>();  // [[unsafe]]
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0>
    struct S {
      template <int IN1, typename TN2, int IN3>
      TN2 getTN2();
    };

    void target(S<int> p) {
      *p.getTN2<0, int *, 1>();
      *p.getTN2<2147483647, int *_Nonnull, -2147483647>();
      *p.getTN2<4, int *_Nullable, 4>();  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest,
     ClassTemplateInstantiationWithTemplateStructsAsParameters) {
  // Class template with another class template as parameter
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    struct Struct2Arg {
      T0 arg0;
      T1 arg1;
    };

    template <typename TN0, typename TN1>
    struct Struct2ArgNested {
      Struct2Arg<TN1, Struct2Arg<TN0, TN1>> *arg0;
      Struct2Arg<TN1, Struct2Arg<TN0, TN1>> *_Nullable arg1;
    };

    void target(Struct2ArgNested<int *_Nonnull, double *_Nullable> p) {
      *p.arg0;
      *p.arg1;  // [[unsafe]]

      *p.arg0->arg0;  // [[unsafe]]
      *p.arg0->arg1.arg0;
      *p.arg0->arg1.arg1;  // [[unsafe]]
    }
  )cc"));

  // Class template with itself as parameter
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    struct Struct2Arg {
      T0 arg0;
      T1 arg1;
    };

    void target(Struct2Arg<Struct2Arg<int *, int *_Nullable>, int *_Nonnull> p) {
      *p.arg0.arg0;
      *p.arg0.arg1;  // [[unsafe]]
      *p.arg1;
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1, typename T2, typename T3, typename T4>
    struct Struct5Arg {
      T0 arg0;
      T1 arg1;
      T2 arg2;
      T3 arg3;
      T4 arg4;
    };

    void target(Struct5Arg<
                Struct5Arg<Struct5Arg<Struct5Arg<int *_Nullable, int *_Nonnull,
                                                 float *_Nullable, int *,
                                                 double *_Nullable>,
                                      int, int, int, int *_Nullable>,
                           int, int *_Nullable, int, int>,
                int, int *_Nullable, int *_Nonnull, int>
                    p) {
      *p.arg0.arg0.arg0.arg0;  // [[unsafe]]
      *p.arg0.arg0.arg0.arg1;
      *p.arg0.arg0.arg0.arg2;  // [[unsafe]]
      *p.arg0.arg0.arg0.arg3;
      *p.arg0.arg0.arg0.arg4;  // [[unsafe]]
      *p.arg0.arg0.arg4;       // [[unsafe]]
      *p.arg0.arg2;            // [[unsafe]]
      *p.arg2;                 // [[unsafe]]
      *p.arg3;
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <int I0, typename T1, typename T2, typename T3, int I4,
              typename T5, typename T6>
    struct Struct7ArgWithInt {
      T1 arg1;
      T2 arg2;
      T3 arg3;
      T5 arg5;
      T6 arg6;
    };

    void target(Struct7ArgWithInt<
                0,
                Struct7ArgWithInt<
                    2147483647,
                    Struct7ArgWithInt<
                        0,
                        Struct7ArgWithInt<-2147483647, int *_Nullable,
                                          int *_Nonnull, float *_Nullable, 0,
                                          int *, double *_Nullable>,
                        int, int, 1, int, int *_Nullable>,
                    int, int *_Nullable, 2147483647, int, int>,
                int, int *_Nullable, 2, int *_Nonnull, int>
                    p) {
      *p.arg1.arg1.arg1.arg1;  // [[unsafe]]
      *p.arg1.arg1.arg1.arg2;
      *p.arg1.arg1.arg1.arg3;  // [[unsafe]]
      *p.arg1.arg1.arg1.arg5;
      *p.arg1.arg1.arg1.arg6;  // [[unsafe]]
      *p.arg1.arg1.arg6;       // [[unsafe]]
      *p.arg1.arg3;            // [[unsafe]]
      *p.arg3;                 // [[unsafe]]
      *p.arg5;
    }
  )cc"));
}

TEST(PointerNullabilityTest,
     ClassTemplateInstantiationWithPointersToStructsAsParameters) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct3IntPtrs {
      int *unknown;
      int *_Nullable nullable;
      int *_Nonnull nonnull;

      int *getUnknown();
      int *_Nullable getNullable();
      int *_Nonnull getNonnull();
    };

    template <typename T0>
    struct Struct1Arg {
      T0 arg0;

      T0 getT0();
    };

    void target(Struct1Arg<Struct3IntPtrs *> p) {
      *p.arg0->unknown;
      *p.arg0->nullable;  // [[unsafe]]
      *p.arg0->nonnull;

      *p.arg0->getUnknown();
      *p.arg0->getNullable();  // [[unsafe]]
      *p.arg0->getNonnull();

      *p.getT0()->unknown;
      *p.getT0()->nullable;  // [[unsafe]]
      *p.getT0()->nonnull;

      *p.getT0()->getUnknown();
      *p.getT0()->getNullable();  // [[unsafe]]
      *p.getT0()->getNonnull();
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct3IntPtrs {
      int *unknown;
      int *_Nullable nullable;
      int *_Nonnull nonnull;

      int *getUnknown();
      int *_Nullable getNullable();
      int *_Nonnull getNonnull();
    };

    template <typename T0>
    struct Struct1Arg {
      T0 arg0;

      T0 getT0();
    };

    void target(Struct1Arg<Struct3IntPtrs *_Nullable> p) {
      *p.arg0->unknown;   // [[unsafe]]
      *p.arg0->nullable;  // [[unsafe]]
      *p.arg0->nonnull;   // [[unsafe]]

      *p.arg0->getUnknown();   // [[unsafe]]
      *p.arg0->getNullable();  // [[unsafe]]
      *p.arg0->getNonnull();   // [[unsafe]]

      *p.getT0()->unknown;   // [[unsafe]]
      *p.getT0()->nullable;  // [[unsafe]]
      *p.getT0()->nonnull;   // [[unsafe]]

      *p.getT0()->getUnknown();   // [[unsafe]]
      *p.getT0()->getNullable();  // [[unsafe]]
      *p.getT0()->getNonnull();   // [[unsafe]]
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct3IntPtrs {
      int *unknown;
      int *_Nullable nullable;
      int *_Nonnull nonnull;

      int *getUnknown();
      int *_Nullable getNullable();
      int *_Nonnull getNonnull();
    };

    template <typename T0>
    struct Struct1Arg {
      T0 arg0;

      T0 getT0();
    };

    void target(Struct1Arg<Struct3IntPtrs *_Nonnull> p) {
      *p.arg0->unknown;
      *p.arg0->nullable;  // [[unsafe]]
      *p.arg0->nonnull;

      *p.arg0->getUnknown();
      *p.arg0->getNullable();  // [[unsafe]]
      *p.arg0->getNonnull();

      *p.getT0()->unknown;
      *p.getT0()->nullable;  // [[unsafe]]
      *p.getT0()->nonnull;

      *p.getT0()->getUnknown();
      *p.getT0()->getNullable();  // [[unsafe]]
      *p.getT0()->getNonnull();
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct Struct3IntPtrs {
      int *unknown;
      int *_Nullable nullable;
      int *_Nonnull nonnull;

      int *getUnknown();
      int *_Nullable getNullable();
      int *_Nonnull getNonnull();
    };

    template <int I0, typename T1>
    struct Struct2Arg {
      T1 arg1;

      T1 getT1();
    };

    void target(Struct2Arg<0, Struct3IntPtrs *> p) {
      *p.arg1->unknown;
      *p.arg1->nullable;  // [[unsafe]]
      *p.arg1->nonnull;

      *p.arg1->getUnknown();
      *p.arg1->getNullable();  // [[unsafe]]
      *p.arg1->getNonnull();

      *p.getT1()->unknown;
      *p.getT1()->nullable;  // [[unsafe]]
      *p.getT1()->nonnull;
      *p.getT1()->getUnknown();
      *p.getT1()->getNullable();  // [[unsafe]]
      *p.getT1()->getNonnull();
    }
  )cc"));
}

TEST(PointerNullabilityTest,
     ClassTemplateInstantiationWithPointersToTemplateStructsAsParameters) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    struct Struct2Arg {
      T0 arg0;
      T1 arg1;

      T0 getT0();
      T1 getT1();
    };

    void target(Struct2Arg<Struct2Arg<int *, int *_Nullable> *_Nullable,
                           Struct2Arg<int, int *> *_Nonnull>
                    p) {
      *p.arg0;        // [[unsafe]]
      *p.arg0->arg0;  // [[unsafe]]
      *p.arg0->arg1;  // [[unsafe]]
      *p.arg1;
      *p.arg1->arg1;

      *p.arg0->getT0();  // [[unsafe]]
      *p.arg0->getT1();  // [[unsafe]]
      *p.arg1->getT1();

      *p.getT0();        // [[unsafe]]
      *p.getT0()->arg0;  // [[unsafe]]
      *p.getT0()->arg1;  // [[unsafe]]
      *p.getT1();
      *p.getT1()->arg1;

      *p.getT0()->getT0();  // [[unsafe]]
      *p.getT0()->getT1();  // [[unsafe]]
      *p.getT1()->getT1();
    }
  )cc"));

  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    struct StructNonnullUnknown {
      T0 nonnull;
      T1 unknown;

      T0 getNonnull();
      T1 getUnknown();
    };

    template <typename T0, typename T1>
    struct StructNonnullNullable {
      T0 nonnull;
      T1 nullable;

      T0 getNonnull();
      T1 getNullable();
    };

    template <typename T0, typename T1>
    struct StructNullableNonnull {
      T0 nullable;
      T1 nonnull;

      T0 getNullable();
      T1 getNonnull();
    };

    template <typename T0, typename T1>
    struct StructNullableNullable {
      T0 nullable0;
      T1 nullable1;

      T0 getNullable0();
      T1 getNullable1();
    };

    template <typename T0, typename T1>
    struct StructNullableUnknown {
      T0 nullable;
      T1 unknown;

      T0 getNullable();
      T1 getUnknown();
    };

    template <typename T0, typename T1>
    struct StructUnknownNullable {
      T0 unknown;
      T1 nullable;

      T0 getUnknown();
      T1 getNullable();
    };

    void
    target(StructNonnullUnknown<
           StructNonnullNullable<
               StructNullableNullable<int *_Nullable, int *_Nullable> *_Nonnull,
               StructUnknownNullable<int *, int *_Nullable> *_Nullable>
               *_Nonnull,
           StructUnknownNullable<
               StructUnknownNullable<int *, int *_Nullable> *,
               StructNullableNonnull<int *_Nullable, int *_Nonnull> *_Nullable>
               *>
               p) {
      *p.nonnull;
      *p.nonnull->nonnull;
      *p.nonnull->nonnull->nullable0;  // [[unsafe]]
      *p.nonnull->nonnull->nullable1;  // [[unsafe]]
      *p.nonnull->nullable;            // [[unsafe]]
      *p.nonnull->nullable->unknown;   // [[unsafe]]
      *p.nonnull->nullable->nullable;  // [[unsafe]]
      *p.unknown->unknown;
      *p.unknown->unknown->unknown;
      *p.unknown->unknown->nullable;  // [[unsafe]]
      *p.unknown;
      *p.unknown->nullable;            // [[unsafe]]
      *p.unknown->nullable->nullable;  // [[unsafe]]
      *p.unknown->nullable->nonnull;   // [[unsafe]]

      *p.nonnull->getNonnull();
      *p.nonnull->getNonnull()->nullable0;  // [[unsafe]]
      *p.nonnull->getNonnull()->nullable1;  // [[unsafe]]
      *p.nonnull->getNullable();            // [[unsafe]]
      *p.nonnull->getNullable()->unknown;   // [[unsafe]]
      *p.nonnull->getNullable()->nullable;  // [[unsafe]]
      *p.unknown->getUnknown();
      *p.unknown->getUnknown()->unknown;
      *p.unknown->getUnknown()->nullable;   // [[unsafe]]
      *p.unknown->getNullable();            // [[unsafe]]
      *p.unknown->getNullable()->nullable;  // [[unsafe]]
      *p.unknown->getNullable()->nonnull;   // [[unsafe]]

      *p.nonnull->getNonnull()->getNullable0();  // [[unsafe]]
      *p.nonnull->getNonnull()->getNullable1();  // [[unsafe]]
      *p.nonnull->getNullable()->getUnknown();   // [[unsafe]]
      *p.nonnull->getNullable()->getNullable();  // [[unsafe]]
      *p.unknown->getUnknown()->getUnknown();
      *p.unknown->getUnknown()->getNullable();   // [[unsafe]]
      *p.unknown->getNullable()->getNullable();  // [[unsafe]]
      *p.unknown->getNullable()->getNonnull();   // [[unsafe]]

      *p.nonnull->nonnull->getNullable0();  // [[unsafe]]
      *p.nonnull->nonnull->getNullable1();  // [[unsafe]]
      *p.nonnull->nullable->getUnknown();   // [[unsafe]]
      *p.nonnull->nullable->getNullable();  // [[unsafe]]
      *p.unknown->unknown->getUnknown();
      *p.unknown->unknown->getNullable();   // [[unsafe]]
      *p.unknown->nullable->getNullable();  // [[unsafe]]
      *p.unknown->nullable->getNonnull();   // [[unsafe]]

      *p.getNonnull();
      *p.getNonnull()->nonnull;
      *p.getNonnull()->nonnull->nullable0;  // [[unsafe]]
      *p.getNonnull()->nonnull->nullable1;  // [[unsafe]]
      *p.getNonnull()->nullable;            // [[unsafe]]
      *p.getNonnull()->nullable->unknown;   // [[unsafe]]
      *p.getNonnull()->nullable->nullable;  // [[unsafe]]
      *p.getUnknown()->unknown;
      *p.getUnknown()->unknown->unknown;
      *p.getUnknown()->unknown->nullable;  // [[unsafe]]
      *p.getUnknown();
      *p.getUnknown()->nullable;            // [[unsafe]]
      *p.getUnknown()->nullable->nullable;  // [[unsafe]]
      *p.getUnknown()->nullable->nonnull;   // [[unsafe]]

      *p.getNonnull()->getNonnull();
      *p.getNonnull()->getNonnull()->nullable0;  // [[unsafe]]
      *p.getNonnull()->getNonnull()->nullable1;  // [[unsafe]]
      *p.getNonnull()->getNullable();            // [[unsafe]]
      *p.getNonnull()->getNullable()->unknown;   // [[unsafe]]
      *p.getNonnull()->getNullable()->nullable;  // [[unsafe]]
      *p.getUnknown()->getUnknown();
      *p.getUnknown()->getUnknown()->unknown;
      *p.getUnknown()->getUnknown()->nullable;   // [[unsafe]]
      *p.getUnknown()->getNullable();            // [[unsafe]]
      *p.getUnknown()->getNullable()->nullable;  // [[unsafe]]
      *p.getUnknown()->getNullable()->nonnull;   // [[unsafe]]

      *p.getNonnull()->nonnull->getNullable0();  // [[unsafe]]
      *p.getNonnull()->nonnull->getNullable1();  // [[unsafe]]
      *p.getNonnull()->nullable->getUnknown();   // [[unsafe]]
      *p.getNonnull()->nullable->getNullable();  // [[unsafe]]
      *p.getUnknown()->unknown->getUnknown();
      *p.getUnknown()->unknown->getNullable();   // [[unsafe]]
      *p.getUnknown()->nullable->getNullable();  // [[unsafe]]
      *p.getUnknown()->nullable->getNonnull();   // [[unsafe]]

      *p.getNonnull()->getNonnull()->getNullable0();  // [[unsafe]]
      *p.getNonnull()->getNonnull()->getNullable1();  // [[unsafe]]
      *p.getNonnull()->getNullable()->getUnknown();   // [[unsafe]]
      *p.getNonnull()->getNullable()->getNullable();  // [[unsafe]]
      *p.getUnknown()->getUnknown()->getUnknown();
      *p.getUnknown()->getUnknown()->getNullable();   // [[unsafe]]
      *p.getUnknown()->getNullable()->getNullable();  // [[unsafe]]
      *p.getUnknown()->getNullable()->getNonnull();   // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, FunctionTemplates) {
  // Call expression that returns the first of two type parameters.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    T0 returnFirst();

    void target() {
      *returnFirst<int *_Nonnull, int *_Nullable>();
      *returnFirst<int *, int *_Nullable>();
      *returnFirst<int *_Nullable, int *_Nonnull>();  // [[unsafe]]
    }
  )cc"));

  // Call expression that returns the second of two type parameters.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T0, typename T1>
    T1 returnSecond();

    void target() {
      *returnSecond<int *_Nullable, int *_Nonnull>();
      *returnSecond<int *_Nullable, int *>();
      *returnSecond<int *, int *_Nullable>();  // [[unsafe]]
    }
  )cc"));

  // Call expression that has an int parameter and two type parameters,
  // returning the first type parameter.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <int I0, typename T1, typename T2>
    T1 fn3ArgWithInt();

    void target() {
      *fn3ArgWithInt<1, int *_Nullable, int *>();  // [[unsafe]]
      *fn3ArgWithInt<1, int *, int *_Nullable>();
    }
  )cc"));

  // Call expression with template parameter substituted with a concrete struct.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    struct StructUnknownNullable {
      int *var0;
      int *_Nullable var1;

      int *getVar0();
      int *_Nullable getVar1();
    };

    template <typename T0, typename T1>
    T1 returnSecond();

    void target() {
      *returnSecond<StructUnknownNullable, int *_Nullable>();  // [[unsafe]]
      *returnSecond<int *_Nonnull, StructUnknownNullable *>();
      *returnSecond<int *_Nonnull, StructUnknownNullable>().var0;
      __assert_nullability<NK_unspecified>(
          returnSecond<int *_Nonnull, StructUnknownNullable>().var0);
      *returnSecond<int *_Nonnull, StructUnknownNullable>().var1;  // [[unsafe]]
      *returnSecond<int *_Nonnull, StructUnknownNullable>().getVar0();
      *returnSecond<int *_Nonnull, StructUnknownNullable>()  // [[unsafe]]
           .getVar1();
    }
  )cc"));
}

TEST(PointerNullabilityTest, OperatorSubscript) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    struct Vec {
      typedef T value_type;
      typedef value_type& reference;
      typedef const value_type& const_reference;

      reference operator[](int n);
      reference at(int n);
      void push_back(const_reference v);
    };

    void target(Vec<int* _Nullable> nullable, Vec<int* _Nonnull> nonnull) {
      // read
      *nullable[0];     // [[unsafe]]
      *nullable.at(0);  // [[unsafe]]
      *nonnull[0];
      *nonnull.at(0);

      // write
      nullable[0] = nullptr;
      nullable.push_back(nullptr);
      nonnull[0] = nullptr;        // [[unsafe]]
      nonnull.push_back(nullptr);  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, OperatorSubscriptPointerIndexType) {
  // Try a case where there is more than one pointer type in the method
  // signature (e.g., the index is nonnull while the return type is nullable)
  // to check we aren't mixing up the nullability vector slots.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    struct Vec {
      typedef T value_type;
      typedef value_type& reference;
      typedef const value_type& const_reference;

      reference operator[](int* _Nonnull p);
      reference at(int* _Nonnull p);
    };

    void target(Vec<int* _Nullable> nullable, Vec<int* _Nonnull> nonnull) {
      int x;
      // read
      *nullable[&x];     // [[unsafe]]
      *nullable.at(&x);  // [[unsafe]]
      *nonnull[&x];
      *nonnull.at(&x);

      // write
      nullable[&x] = nullptr;
      nonnull[&x] = nullptr;  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, OperatorStarAndEquals) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    struct Optional {
      using value_type = T;

      const value_type& operator*() const&;
      const value_type& value() const&;
      Optional<T>& operator=(const value_type& v);
    };

    // Try a non-member operator* too, to make sure we don't crash.
    template <typename T>
    static Optional<T> operator*(const Optional<T>& L, const Optional<T>& R);

    void target(Optional<int* _Nullable> nullable,
                Optional<int* _Nonnull> nonnull) {
      // read
      *(*nullable);         // [[unsafe]]
      *(nullable.value());  // [[unsafe]]
      *(*nonnull);
      *(nonnull.value());

      // write
      nullable = nullptr;
      // TODO(b/405355053): This should be unsafe but we are only getting
      // the return type from resugaring member operator calls. We aren't yet
      // get the argument nullability attached to the parameter of each
      // instance yet (compare to `CallInstantiatedMember` test below for
      // non-operator member functions).
      nonnull = nullptr;

      // non-member operator*
      Optional<int* _Nullable> nullable2 = nullable * nullable;
      Optional<int* _Nullable> nonnull2 = nonnull * nonnull;
    }
  )cc"));
}

TEST(PointerNullabilityTest, ParenTypeInTemplate1) {
  checkDiagnostics(R"cc(
    template <typename T>
    struct S {
      T(a);
      T(*(b));

      T (*f)();
      T(((*g)))();
    };

    void target(S<int *_Nullable> s) {
      *s.a;   // [[unsafe]]
      **s.b;  // [[unsafe]]
      *s.f;
      *s.g;
      *s.f();  // [[unsafe]]
      *s.g();  // [[unsafe]]
    }
  )cc");
}

TEST(PointerNullabilityTest, ParenTypeInTemplate2) {
  checkDiagnostics(R"cc(
    template <typename T>
    struct S {
      T(a);
      T(*(b));

      T (*f)();
      T(((*g)))();
    };

    void target(S<int *_Nonnull> s) {
      *s.a;
      **s.b;
      *s.f;
      *s.g;
      *s.f();
      *s.g();
    }
  )cc");
}

TEST(PointerNullabilityTest, ParenTypeInTemplate3) {
  checkDiagnostics(R"cc(
    template <typename T>
    struct S {
      T arg;
    };

    void target(S<int *_Nullable>(a), S<int *_Nullable>(*(b)),
                S<int(*_Nullable)> c, S<int *(*(*_Nullable))> d,
                S<int *(*_Nullable)()> e) {
      *a.arg;    // [[unsafe]]
      *b->arg;   // [[unsafe]]
      *c.arg;    // [[unsafe]]
      ***d.arg;  // [[unsafe]]
      *e.arg;    // [[unsafe]]

      *e.arg();  // [[unsafe]]
    }
  )cc");
}

TEST(PointerNullabilityTest, ParenTypeInTemplate4) {
  checkDiagnostics(R"cc(
    template <typename T>
    struct S {
      T arg;
    };

    void target(S<int *_Nonnull>(a), S<int *_Nonnull>(*(b)),
                S<int(*_Nonnull)> c, S<int *(*(*_Nonnull))> d,
                S<int *_Nonnull (*)()> e) {
      *a.arg;
      *b->arg;
      *c.arg;
      ***d.arg;
      *e.arg;

      *e.arg();
    }
  )cc");
}

TEST(PointerNullabilityTest, PartialSpecialization) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <int, class>
    struct S;
    template <class T>
    struct S<0, T> {
      using Alias = T;
    };

    template <int i, class T>
    typename S<i, T>::Alias f(T);

    void target(int *p) { (void *)f<0>(p); }
  )cc"));
}

TEST(PointerNullabilityTest, ClassTemplateWithDefaultArgument) {
  // Crash repro.
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <class T1, class T2 = T1>
    struct S {
     public:
      void f(S<T2>);
    };
    void target() {
      S<int *_Nullable> s;
      s.f(s);
    }
  )cc"));
}

TEST(PointerNullabilityTest, MethodOnPartialSpecialization) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <class T>
    struct S {};
    template <class T1, class T2>
    struct pair {};
    template <class T1, class T2>
    struct S<pair<T1, T2>> {
      T1 Foo(T1, T2);
    };
    void target(int* _Nonnull p1, char* _Nullable p2) {
      S<pair<int* _Nonnull, char* _Nullable>> s;
      // TODO: Should be NK_nonnull, but we don't treat partial specializations
      // correctly yet.
      __assert_nullability<NK_unspecified>(s.Foo(p1, p2));
    }
  )cc"));
}

TEST(PointerNullabilityTest, CallMethodTakingParameterPack) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <class... Ts>
    struct S {
      S<Ts...> Foo(Ts...);
    };
    void target(int* _Nonnull p1, char* _Nullable p2) {
      S<int* _Nonnull, char* _Nullable> s;
      // TODO: Should be [NK_nonnull, NK_nullable], but we don't treat parameter
      // packs correctly yet.
      __assert_nullability<NK_unspecified, NK_unspecified>(s.Foo(p1, p2));
    }
  )cc"));
}

TEST(PointerNullabilityTest, CallFunctionReturningTemplateSpecializationType) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <class T>
    struct AliasWrapper {
      using Type = T;
    };
    template <typename>
    AliasWrapper<int* _Nonnull>::Type f();
    void target() { __assert_nullability<NK_nonnull>(f<int>()); }
  )cc"));
}

TEST(PointerNullabilityTest, CallInstantiatedMember) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    struct Sink {
      void eat(T);
    };
    void target(Sink<int *_Nonnull> &S) {
      S.eat(nullptr);  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, CallInstantiatedFreeFunction) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    void eat(T p);

    void target() {
      eat<int *_Nullable>(nullptr);
      eat<int *_Nonnull>(nullptr);  // [[unsafe]]
    }
  )cc"));
}

TEST(PointerNullabilityTest, CallAnnotatedTemplateParam) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    void eat(T *_Nonnull p);

    void target(int *_Nullable p, int *_Nonnull q) {
      eat(p);  // [[unsafe]]
      eat(q);
    }
  )cc"));
}

// Tests for when the "target" itself is a templated function.

// Case that is unsafe independent of the template arguments (already annotated)
TEST(PointerNullabilityTest, BodyIndependentOfTemplateArgsAnnotatedParam) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    void target(T *_Nullable p) {
      T *v = p;
      *v;  // [[unsafe]]
    }

    void instantiation() { target<int>(nullptr); }
  )cc"));
}

TEST(PointerNullabilityTest, BodyIndependentOfTemplateArgAnnotatedCall) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    T *_Nullable getNullable();

    template <typename T>
    void target() {
      T *v = getNullable<T>();
      *v;  // [[unsafe]]
    }

    void instantiation() { target<int>(); }
  )cc"));
}

// Case is unsafe that is partly dependent on the template arguments.
// - The *v is unsafe independently (and we flag it).
// - The T v = nullptr perhaps should be unsafe when T is _Nonnull but
//   we currently do not get access to that annotation information.
TEST(PointerNullabilityTest, BodyPartlyDependentOnTemplateArgs) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    void target() {
      T v = nullptr;
      *v;  // [[unsafe]]
    }

    void instantiation() { target<int *_Nonnull>(); }
  )cc"));
}

// Case that is dependent on the template arguments.
TEST(PointerNullabilityTest, BodyDependentOnTemplateArgs) {
  EXPECT_TRUE(checkDiagnostics(R"cc(
    template <typename T>
    void target(T p) {
      *p;
    }

    void instantiation() {
      // This should make the *p above unsafe, but verification doesn't yet
      // handle this (b/268347355).
      target<int *_Nullable>(nullptr);
    }
  )cc"));
}

}  // namespace
}  // namespace clang::tidy::nullability

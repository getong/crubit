// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CRUBIT_NULLABILITY_INFERENCE_INFERRABLE_H_
#define CRUBIT_NULLABILITY_INFERENCE_INFERRABLE_H_

#include "clang/AST/DeclBase.h"

namespace clang::tidy::nullability {

/// Should we attempt to deduce nullability for this symbol?
bool isInferenceTarget(const Decl &);

/// The number of nullability slots in this symbol's type which can be inferred.
///
/// This may not be all the slots in the type: e.g. `int** X` has outer and
/// inner nullability, we may support only inferring outer.
int countInferrableSlots(const clang::Decl &);

}  // namespace clang::tidy::nullability

#endif  // THIRD_PARTY_CRUBIT_NULLABILITY_INFERENCE_INFERRABLE_H_

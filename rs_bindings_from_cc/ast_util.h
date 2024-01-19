// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CRUBIT_RS_BINDINGS_FROM_CC_AST_UTIL_H_
#define CRUBIT_RS_BINDINGS_FROM_CC_AST_UTIL_H_

#include <optional>
#include <string>

#include "absl/functional/function_ref.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclBase.h"

namespace crubit {

// Returns true if `decl` is either 1) a ClassTemplateSpecializationDecl (but
// not ClassTemplatePartialSpecializationDecl) or 2) a decl (e.g. a member
// function decl) nested inside a ClassTemplateSpecializationDecl.
bool IsFullClassTemplateSpecializationOrChild(const clang::Decl* decl);

// Returns a human-readable string containing the list of unknown attrs.
//
// is_known is called exactly once on every attribute, and returns true if the
// attribute is understood.
std::optional<std::string> CollectUnknownAttrs(
    const clang::Decl& decl,
    absl::FunctionRef<bool(const clang::Attr&)> is_known =
        [](const clang::Attr& attr) { return false; });

}  // namespace crubit

#endif  // CRUBIT_RS_BINDINGS_FROM_CC_AST_UTIL_H_

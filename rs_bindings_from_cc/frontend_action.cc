// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "rs_bindings_from_cc/frontend_action.h"

#include <memory>

#include "lifetime_annotations/lifetime_annotations.h"
#include "rs_bindings_from_cc/ast_consumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/StringRef.h"

namespace crubit {

std::unique_ptr<clang::ASTConsumer> FrontendAction::CreateASTConsumer(
    clang::CompilerInstance& instance, llvm::StringRef) {
  AddLifetimeAnnotationHandlers(instance.getPreprocessor(),
                                invocation_.lifetime_context_);
  return std::make_unique<AstConsumer>(instance, invocation_);
}

}  // namespace crubit

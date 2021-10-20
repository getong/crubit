// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CRUBIT_RS_BINDINGS_FROM_CC_AST_VISITOR_H_
#define CRUBIT_RS_BINDINGS_FROM_CC_AST_VISITOR_H_

#include <memory>
#include <string>

#include "rs_bindings_from_cc/ir.h"
#include "third_party/absl/container/flat_hash_set.h"
#include "third_party/absl/status/statusor.h"
#include "third_party/absl/strings/string_view.h"
#include "third_party/absl/types/span.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/ASTContext.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Decl.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Mangle.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/RawCommentList.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/RecursiveASTVisitor.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Type.h"
#include "third_party/llvm/llvm-project/clang/include/clang/Basic/SourceLocation.h"
#include "third_party/llvm/llvm-project/clang/include/clang/Sema/Sema.h"

namespace rs_bindings_from_cc {

// Iterates over the AST created from `public_header_names` (a collection of
// paths in the format suitable for a google3-relative quote include) and
// creates an intermediate representation of the import (`IR`).
class AstVisitor : public clang::RecursiveASTVisitor<AstVisitor> {
 public:
  using Base = clang::RecursiveASTVisitor<AstVisitor>;

  explicit AstVisitor(clang::Sema& sema,
                      absl::Span<const absl::string_view> public_header_names,
                      IR& ir)
      : sema_(sema),
        public_header_names_(public_header_names),
        ir_(ir),
        ctx_(nullptr),
        comment_manager_(ir) {}

  // These functions are called by the base class while visiting the different
  // parts of the AST. The API follows the rules of the base class which is
  // responsible for the traversal of the AST.
  bool TraverseTranslationUnitDecl(
      clang::TranslationUnitDecl* translation_unit_decl);
  bool TraverseDecl(clang::Decl* decl);

  bool VisitFunctionDecl(clang::FunctionDecl* function_decl);
  bool VisitRecordDecl(clang::RecordDecl* record_decl);

 private:
  std::optional<std::vector<Field>> ImportFields(
      clang::RecordDecl* record_decl, clang::AccessSpecifier default_access);

  std::string GetMangledName(const clang::NamedDecl* named_decl) const;

  // Gets an IR UnqualifiedIdentifier for the named decl.
  //
  // If the decl's name is an identifier, this returns that identifier as-is.
  //
  // If the decl is a special member function or operator overload, this returns
  // a SpecialName.
  //
  // If the translated name is not yet implemented, this returns null.
  std::optional<UnqualifiedIdentifier> GetTranslatedName(
      const clang::NamedDecl* named_decl) const;

  // GetTranslatedName, but only for identifier names. This is the common case.
  std::optional<Identifier> GetTranslatedIdentifier(
      const clang::NamedDecl* named_decl) const {
    if (std::optional<UnqualifiedIdentifier> name =
            GetTranslatedName(named_decl)) {
      return std::move(*std::get_if<Identifier>(&*name));
    }
    return std::nullopt;
  }

  // Gets the doc comment of the declaration.
  std::optional<std::string> GetComment(const clang::Decl* decl) const;
  SourceLoc ConvertSourceLoc(clang::SourceLocation loc) const;
  SourceLoc ConvertSourceLoc(clang::SourceRange range) const;
  absl::StatusOr<MappedType> ConvertType(clang::QualType qual_type) const;

  clang::Sema& sema_;
  absl::Span<const absl::string_view> public_header_names_;
  IR& ir_;
  clang::ASTContext* ctx_;
  std::unique_ptr<clang::MangleContext> mangler_;
  absl::flat_hash_set<const clang::Decl*> seen_decls_;

  // A component that keeps track of all comments and emits IR for all top-level
  // comments that are not doc comments.
  class CommentManager {
   public:
    explicit CommentManager(IR& ir) : ir_(ir) {}

    // Notify the comment manager that we the visitor is traversing a decl.
    // This will emit IR for all preceding comments.
    void TraverseDecl(clang::Decl* decl);

    // Emit IR for the remaining comments after the last decl.
    void FlushComments();

   private:
    void LoadComments();
    void VisitTopLevelComment(clang::RawComment* comment);

    IR& ir_;
    clang::ASTContext* ctx_;
    clang::FileID current_file_;
    std::vector<clang::RawComment*> file_comments_;
    std::vector<clang::RawComment*>::iterator next_comment_;
  };

  CommentManager comment_manager_;
};  // class AstVisitor

}  // namespace rs_bindings_from_cc

#endif  // CRUBIT_RS_BINDINGS_FROM_CC_AST_VISITOR_H_

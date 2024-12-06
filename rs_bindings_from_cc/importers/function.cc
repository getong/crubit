// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "rs_bindings_from_cc/importers/function.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/btree_set.h"
#include "absl/log/check.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "common/annotation_reader.h"
#include "lifetime_annotations/lifetime.h"
#include "lifetime_annotations/lifetime_annotations.h"
#include "lifetime_annotations/lifetime_error.h"
#include "lifetime_annotations/lifetime_symbol_table.h"
#include "lifetime_annotations/type_lifetimes.h"
#include "rs_bindings_from_cc/ast_util.h"
#include "rs_bindings_from_cc/ir.h"
#include "rs_bindings_from_cc/recording_diagnostic_consumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Attrs.inc"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Type.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

namespace crubit {
namespace {

// A collection of `FormattedError` values that enforces uniqueness.
struct Errors {
  // `btree_set` is used to ensure stable ordering.
  absl::btree_set<FormattedError> error_set;
  void Add(FormattedError error) {
    auto result = error_set.insert(std::move(error));
    CHECK(result.second) << "Duplicated error message";
  }
  void AddStatus(absl::Status status) {
    Add(FormattedError::FromStatus(status));
  }
};

SafetyAnnotation GetSafetyAnnotation(const clang::Decl& decl, Errors& errors) {
  absl::StatusOr<const clang::AnnotateAttr*> override_annotation =
      GetAnnotateAttr(decl, "crubit_override_unsafe");
  if (!override_annotation.ok()) {
    errors.AddStatus(override_annotation.status());
    return SafetyAnnotation::kUnannotated;
  }
  if (*override_annotation == nullptr) {
    return SafetyAnnotation::kUnannotated;
  }
  absl::StatusOr<bool> is_unsafe =
      GetAnnotateArgAsBool(**override_annotation, decl.getASTContext());
  if (!override_annotation.ok()) {
    errors.AddStatus(is_unsafe.status());
    return SafetyAnnotation::kUnannotated;
  }
  if (*is_unsafe) {
    return SafetyAnnotation::kUnsafe;
  } else {
    return SafetyAnnotation::kDisableUnsafe;
  }
}

}  // namespace

static bool IsInStdNamespace(const clang::FunctionDecl* decl) {
  const clang::DeclContext* context = decl->getDeclContext();
  while (context) {
    if (context->isStdNamespace()) {
      return true;
    }
    context = context->getParent();
  }
  return false;
}

Identifier FunctionDeclImporter::GetTranslatedParamName(
    const clang::ParmVarDecl* param_decl) {
  int param_pos = param_decl->getFunctionScopeIndex();
  absl::StatusOr<Identifier> name = ictx_.GetTranslatedIdentifier(param_decl);
  if (!name.ok()) {
    return {Identifier(absl::StrCat("__param_", param_pos))};
  }
  if (auto* sttpt =
          param_decl->getType()->getAs<clang::SubstTemplateTypeParmType>();
      sttpt && sttpt->getReplacedParameter()->isParameterPack()) {
    // Avoid giving the same name to all parameters expanded from a pack.
    return {Identifier(absl::StrCat("__", name->Ident(), "_", param_pos))};
  }
  return *name;
}

std::optional<IR::Item> FunctionDeclImporter::Import(
    clang::FunctionDecl* function_decl) {
  if (!ictx_.IsFromCurrentTarget(function_decl)) return std::nullopt;
  if (function_decl->isDeleted()) return std::nullopt;

  clang::FunctionDecl* template_decl_for_method =
      function_decl->getInstantiatedFromMemberFunction();
  if (IsInStdNamespace(function_decl)) {
    if (clang::IdentifierInfo* id = function_decl->getIdentifier();
        id != nullptr && id->getName().find("__") != llvm::StringRef::npos) {
      return ictx_.ImportUnsupportedItem(
          function_decl,
          FormattedError::Static("Internal functions from the standard "
                                 "library are not supported"));
    }
  }
  // Method is private, we don't need to import it.
  if (auto* method_decl =
          clang::dyn_cast<clang::CXXMethodDecl>(function_decl)) {
    switch (method_decl->getAccess()) {
      case clang::AS_public:
        break;
      case clang::AS_protected:
      case clang::AS_private:
      case clang::AS_none:
        // No need for IR to include Func representing private methods.
        // TODO(lukasza): Revisit this for protected methods.
        return std::nullopt;
    }
  }

  // We should only import methods of class template specializations
  // that can be instantiated: the template may spell out the method,
  // but it's not guaranteed to be instantiable for the template parameter(s);
  // importing an un-instantiable method causes Crubit to generate a thunk to
  // invoke this method, which triggers instantiation when compiling the
  // generated bindings, which fails the build.
  if (template_decl_for_method) {
    // Some methods in STL are explicitly marked with
    // `__attribute__((exclude_from_explicit_instantiation))`, and attempt to
    // instantiate them may crash clang, so we skip them for now.
    bool skip_instantiation = false;
    if (template_decl_for_method->hasAttrs()) {
      skip_instantiation = std::any_of(
          function_decl->attr_begin(), function_decl->attr_end(),
          [](auto attr) {
            return clang::isa<clang::ExcludeFromExplicitInstantiationAttr>(
                attr);
          });
    }
    if (!function_decl->isDefined() && !skip_instantiation) {
      // Here, we have the option to instantiate the function
      // definition recursively, that is, to instantiate the function
      // templates invoked within the (templated) body. This checks the
      // validity of the function template more thoroughly than simply
      // ensuring the type of the invoked function template is correct: e.g.,
      // If `Recursive` is set, a diagnostic would be emitted if the function
      // template invoked in the body fails to instantiate (e.g., due to a
      // static_assert) but still passes type checking. However, this has the
      // side effect of actually instantiating the invoked function template
      // and the invoked function template would be considered "defined", so
      // we wouldn't be able to get diagnostics when actually importing the
      // invoked function template.
      // TODO(b/248542210): Propagate the validity check of function templates
      // in a function template body.
      // TODO(b/248542210): `clang::Sema::InstantiateClassMembers` checks more
      // constraints (than here) when instantiating the methods, consider use
      // that API instead (and avoid calling `InstantiateFunctionDefinition`
      // here). We don't use it now because we cannot clearly attribute
      // emitted diagnostics to a member decl (we need diagnostics because the
      // decl may be considered valid `!decl->isInvalidDecl()` while its
      // instantiation may fail.)
      auto point_of_instantiation = function_decl->getPointOfInstantiation();
      // Point of instantiation is invalid if Crubit is eagerly
      // instantiating a method of a class template specialization.
      if (point_of_instantiation.isInvalid()) {
        point_of_instantiation = function_decl->getLocation();
      }
      crubit::RecordingDiagnosticConsumer diagnostic_recorder =
          crubit::RecordDiagnostics(ictx_.sema_.getDiagnostics(), [&] {
            ictx_.sema_.InstantiateFunctionDefinition(point_of_instantiation,
                                                      function_decl);
          });
      std::string diagnostics =
          diagnostic_recorder.ConcatenatedDiagnostics("Diagnostics emitted:\n");
      if (diagnostic_recorder.getNumErrors() != 0) {
        // Clang considers the function decl valid even fatal diagnostics is
        // emitted during instantiation. However, such diagnostics would fail
        // compilation of generated bindings, so it's invalid as far as Crubit
        // is concerned, thus set it as invalid here.
        function_decl->setInvalidDecl();
        return ictx_.ImportUnsupportedItem(
            function_decl,
            FormattedError::PrefixedStrCat(
                "Failed to instantiate the function/method template",
                diagnostics));
      }
    }
  }
  if (function_decl->isInvalidDecl()) {
    return ictx_.ImportUnsupportedItem(
        function_decl,
        FormattedError::Static("Function declaration is considered invalid"));
  }

  clang::tidy::lifetimes::LifetimeSymbolTable lifetime_symbol_table;
  std::optional<clang::tidy::lifetimes::FunctionLifetimes> lifetimes;
  llvm::Expected<clang::tidy::lifetimes::FunctionLifetimes> lifetimes_or_err =
      clang::tidy::lifetimes::GetLifetimeAnnotations(
          function_decl, *ictx_.invocation_.lifetime_context_,
          &lifetime_symbol_table);
  if (lifetimes_or_err) {
    lifetimes = std::move(*lifetimes_or_err);
  } else {
    using clang::tidy::lifetimes::LifetimeError;
    llvm::Error remaining_err = llvm::handleErrors(
        lifetimes_or_err.takeError(),
        [](std::unique_ptr<LifetimeError> lifetime_err) -> llvm::Error {
          switch (lifetime_err->type()) {
            case LifetimeError::Type::ElisionNotEnabled:
            case LifetimeError::Type::CannotElideOutputLifetimes:
              // If elision is not enabled or output lifetimes cannot be
              // elided, we want to import the function with raw lifetime-less
              // pointers. Just return success here; this will leave the
              // `lifetimes` optional empty, and we will then handle this
              // accordingly below.
              return llvm::Error::success();
              break;
            default:
              return llvm::Error(std::move(lifetime_err));
              break;
          }
        });
    if (remaining_err) {
      return ictx_.ImportUnsupportedItem(
          function_decl, FormattedError::PrefixedStrCat(
                             "Unable to get lifetime annotations",
                             llvm::toString(std::move(remaining_err))));
    }
  }

  absl::StatusOr<UnqualifiedIdentifier> translated_name =
      ictx_.GetTranslatedName(function_decl);
  if (!translated_name.ok()) {
    return ictx_.ImportUnsupportedItem(
        function_decl,
        FormattedError::PrefixedStrCat("Function name is not supported",
                                       translated_name.status().message()));
  }

  std::vector<FuncParam> params;
  Errors errors;
  if (auto* method_decl =
          clang::dyn_cast<clang::CXXMethodDecl>(function_decl)) {
    if (!ictx_.HasBeenAlreadySuccessfullyImported(method_decl->getParent())) {
      return ictx_.ImportUnsupportedItem(
          function_decl, FormattedError::Static("Couldn't import the parent"));
    }

    // non-static member functions receive an implicit `this` parameter.
    if (method_decl->isInstance()) {
      const clang::tidy::lifetimes::ValueLifetimes* this_lifetimes = nullptr;
      if (lifetimes) {
        this_lifetimes = &lifetimes->GetThisLifetimes();
      }
      auto param_type =
          ictx_.ConvertQualType(method_decl->getThisType(), this_lifetimes,
                                std::optional<clang::RefQualifierKind>(
                                    method_decl->getRefQualifier()),
                                /*nullable=*/false);
      if (!param_type.ok()) {
        errors.Add(
            FormattedError::PrefixedStrCat("`this` parameter is not supported",
                                           param_type.status().message()));
      } else {
        params.push_back(
            {.type = *std::move(param_type),
             .identifier = Identifier("__this"),
             // TODO(b/319524852): catch `[[clang::lifetimbound]]` on `this`.
             .unknown_attr = {}});
      }
    }
  }

  if (lifetimes) {
    CHECK(lifetimes->IsValidForDecl(function_decl));
  }

  for (unsigned i = 0; i < function_decl->getNumParams(); ++i) {
    const clang::ParmVarDecl* param = function_decl->getParamDecl(i);
    const clang::tidy::lifetimes::ValueLifetimes* param_lifetimes = nullptr;
    if (lifetimes) {
      param_lifetimes = &lifetimes->GetParamLifetimes(i);
    }
    auto param_type =
        ictx_.ConvertQualType(param->getType(), param_lifetimes, std::nullopt);
    if (!param_type.ok()) {
      errors.Add(
          FormattedError::Substitute("Parameter #$0 is not supported: $1", i,
                                     param_type.status().message()));
      continue;
    }

    std::optional<Identifier> param_name = GetTranslatedParamName(param);
    CHECK(param_name.has_value());  // No known failure cases.
    params.push_back({.type = *param_type,
                      .identifier = *std::move(param_name),
                      .unknown_attr = CollectUnknownAttrs(*param)});
  }

  bool undeduced_return_type =
      function_decl->getReturnType()->isUndeducedType();
  if (undeduced_return_type) {
    // Use a custom diagnoser as the `DeduceReturnType` call may fail, which
    // is OK if this is a method of a class template, since Crubit
    // instantiates the members of the class templates eagerly.
    crubit::RecordingDiagnosticConsumer diagnostic_recorder =
        crubit::RecordDiagnostics(ictx_.sema_.getDiagnostics(), [&] {
          undeduced_return_type = ictx_.sema_.DeduceReturnType(
              function_decl, function_decl->getLocation());
        });
    if (undeduced_return_type) {
      errors.Add(FormattedError::PrefixedStrCat(
          "Couldn't deduce the return type",
          diagnostic_recorder.ConcatenatedDiagnostics(
              "Diagnostics emitted:\n")));
    }
  }
  absl::StatusOr<MappedType> return_type;
  if (!undeduced_return_type) {
    const clang::tidy::lifetimes::ValueLifetimes* return_lifetimes = nullptr;
    if (lifetimes) {
      return_lifetimes = &lifetimes->GetReturnLifetimes();
    }
    return_type = ictx_.ConvertQualType(function_decl->getReturnType(),
                                        return_lifetimes, std::nullopt);
    if (!return_type.ok()) {
      errors.Add(FormattedError::PrefixedStrCat(
          "Return type is not supported", return_type.status().message()));
    }
  }

  llvm::DenseSet<clang::tidy::lifetimes::Lifetime> all_free_lifetimes;
  if (lifetimes) {
    all_free_lifetimes = lifetimes->AllFreeLifetimes();
  }

  std::vector<LifetimeName> lifetime_params;
  for (clang::tidy::lifetimes::Lifetime lifetime : all_free_lifetimes) {
    std::optional<llvm::StringRef> name =
        lifetime_symbol_table.LookupLifetime(lifetime);
    CHECK(name.has_value());
    lifetime_params.push_back(
        {.name = name->str(), .id = LifetimeId(lifetime.Id())});
  }
  llvm::sort(lifetime_params,
             [](const LifetimeName& l1, const LifetimeName& l2) {
               return l1.name < l2.name;
             });

  std::optional<MemberFuncMetadata> member_func_metadata;
  if (auto* method_decl =
          clang::dyn_cast<clang::CXXMethodDecl>(function_decl)) {
    std::optional<MemberFuncMetadata::InstanceMethodMetadata> instance_metadata;
    if (method_decl->isInstance()) {
      MemberFuncMetadata::ReferenceQualification reference;
      switch (method_decl->getRefQualifier()) {
        case clang::RQ_LValue:
          reference = MemberFuncMetadata::kLValue;
          break;
        case clang::RQ_RValue:
          reference = MemberFuncMetadata::kRValue;
          break;
        case clang::RQ_None:
          reference = MemberFuncMetadata::kUnqualified;
          break;
      }
      instance_metadata = MemberFuncMetadata::InstanceMethodMetadata{
          .reference = reference,
          .is_const = method_decl->isConst(),
          .is_virtual = method_decl->isVirtual(),
      };
    }

    member_func_metadata = MemberFuncMetadata{
        .record_id = ictx_.GenerateItemId(method_decl->getParent()),
        .instance_method_metadata = instance_metadata};
  }

  if (!errors.error_set.empty()) {
    return ictx_.ImportUnsupportedItem(
        function_decl,
        std::vector(errors.error_set.begin(), errors.error_set.end()));
  }

  bool has_c_calling_convention =
      function_decl->getType()->getAs<clang::FunctionType>()->getCallConv() ==
      clang::CC_C;
  bool is_member_or_descendant_of_class_template =
      IsFullClassTemplateSpecializationOrChild(function_decl);

  SafetyAnnotation safety_annotation =
      GetSafetyAnnotation(*function_decl, errors);

  std::optional<std::string> doc_comment = ictx_.GetComment(function_decl);
  if (!doc_comment.has_value() && is_member_or_descendant_of_class_template) {
    // Despite `is_member_or_descendant_of_class_template` check above, we are
    // not guaranteed that a `func_pattern` exists below.  For example, it may
    // be missing when `function_decl` is an implicitly defined constructor of
    // a class template -- such decls are generated, not instantiated.
    if (clang::FunctionDecl* func_pattern =
            function_decl->getTemplateInstantiationPattern()) {
      doc_comment = ictx_.GetComment(func_pattern);
    }
  }

  std::optional<std::string> nodiscard;
  std::optional<std::string> deprecated;
  std::optional<std::string> unknown_attr =
      CollectUnknownAttrs(*function_decl, [&](const clang::Attr& attr) {
        if (auto* unused_attr =
                clang::dyn_cast<clang::WarnUnusedResultAttr>(&attr)) {
          nodiscard.emplace(unused_attr->getMessage());
          return true;
        } else if (auto* deprecated_attr =
                       clang::dyn_cast<clang::DeprecatedAttr>(&attr)) {
          deprecated.emplace(deprecated_attr->getMessage());
          return true;
        } else if (clang::isa<clang::NoReturnAttr>(attr)) {
          return true;  // we call isNoReturn below, instead
        } else if (clang::isa<clang::NoThrowAttr>(attr)) {
          // nothrow attributes don't affect Rust.
          return true;
        }
        return false;
      });

  // Silence ClangTidy, checked above: calling `errors.Add` if
  // `!return_type.ok()` and returning early if `!errors.empty()`.
  CHECK_OK(return_type);

  auto enclosing_item_id = ictx_.GetEnclosingItemId(function_decl);
  if (!enclosing_item_id.ok()) {
    return ictx_.ImportUnsupportedItem(
        function_decl,
        FormattedError::FromStatus(std::move(enclosing_item_id.status())));
  }

  return Func{
      .name = *translated_name,
      .owning_target = ictx_.GetOwningTarget(function_decl),
      .doc_comment = std::move(doc_comment),
      .mangled_name = ictx_.GetMangledName(function_decl),
      .return_type = *return_type,
      .params = std::move(params),
      .lifetime_params = std::move(lifetime_params),
      .is_inline = function_decl->isInlined(),
      .member_func_metadata = std::move(member_func_metadata),
      .is_extern_c = function_decl->isExternC(),
      .is_noreturn = function_decl->isNoReturn(),
      .nodiscard = std::move(nodiscard),
      .deprecated = std::move(deprecated),
      .unknown_attr = std::move(unknown_attr),
      .has_c_calling_convention = has_c_calling_convention,
      .is_member_or_descendant_of_class_template =
          is_member_or_descendant_of_class_template,
      .safety_annotation = safety_annotation,
      .source_loc = ictx_.ConvertSourceLocation(function_decl->getBeginLoc()),
      .id = ictx_.GenerateItemId(function_decl),
      .enclosing_item_id = *std::move(enclosing_item_id),
  };
}

}  // namespace crubit

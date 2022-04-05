// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lifetime_annotations/type_lifetimes.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "third_party/absl/strings/str_cat.h"
#include "third_party/absl/strings/str_format.h"
#include "third_party/absl/strings/str_join.h"
#include "lifetime_annotations/function_lifetimes.h"
#include "lifetime_annotations/lifetime.h"
#include "lifetime_annotations/lifetime_symbol_table.h"
#include "lifetime_annotations/pointee_type.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Attr.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Attrs.inc"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/DeclCXX.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/DeclTemplate.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/TemplateBase.h"
#include "third_party/llvm/llvm-project/clang/include/clang/AST/Type.h"
#include "third_party/llvm/llvm-project/clang/include/clang/Basic/LLVM.h"
#include "third_party/llvm/llvm-project/clang/include/clang/Basic/SourceLocation.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/ArrayRef.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/DenseMapInfo.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/Hashing.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/SmallVector.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/ADT/StringRef.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/Support/Error.h"
#include "third_party/llvm/llvm-project/llvm/include/llvm/Support/ErrorHandling.h"

namespace devtools_rust {

llvm::SmallVector<std::string> GetLifetimeParameters(clang::QualType type) {
  // TODO(mboehme):
  // - Add support for type aliases with lifetime parameters
  // - Report errors as Clang diagnostics, not through
  //   llvm::report_fatal_error().

  auto record = type->getAs<clang::RecordType>();
  if (!record) {
    return {};
  }

  auto cxx_record = record->getAsCXXRecordDecl();
  if (!cxx_record) {
    return {};
  }

  const clang::AnnotateAttr* lifetime_params_attr = nullptr;
  for (auto annotate : cxx_record->specific_attrs<clang::AnnotateAttr>()) {
    if (annotate->getAnnotation() == "lifetime_params") {
      if (lifetime_params_attr) {
        llvm::report_fatal_error("repeated lifetime annotation");
      }
      lifetime_params_attr = annotate;
    }
  }
  if (!lifetime_params_attr) {
    return {};
  }

  llvm::SmallVector<std::string> ret;
  for (const auto& arg : lifetime_params_attr->args()) {
    llvm::StringRef lifetime;
    if (llvm::Error err =
            EvaluateAsStringLiteral(arg, cxx_record->getASTContext())
                .moveInto(lifetime)) {
      llvm::report_fatal_error(llvm::StringRef(toString(std::move(err))));
    }
    ret.push_back(lifetime.str());
  }

  return ret;
}

ValueLifetimes::ValueLifetimes(const ValueLifetimes& other) { *this = other; }

ValueLifetimes& ValueLifetimes::operator=(const ValueLifetimes& other) {
  type_ = other.type_;
  template_argument_lifetimes_ = other.template_argument_lifetimes_;
  lifetime_parameters_by_name_ = other.lifetime_parameters_by_name_;
  pointee_lifetimes_ =
      other.pointee_lifetimes_
          ? std::make_unique<ObjectLifetimes>(*other.pointee_lifetimes_)
          : nullptr;
  function_lifetimes_ =
      other.function_lifetimes_
          ? std::make_unique<FunctionLifetimes>(*other.function_lifetimes_)
          : nullptr;
  return *this;
}

// Defined here because FunctionLifetimes is an incomplete type in the header.
ValueLifetimes::~ValueLifetimes() = default;

namespace {

llvm::Error ForEachTemplateArgument(
    clang::QualType type,
    const std::function<llvm::Error(int, clang::QualType)>& callback) {
  llvm::SmallVector<llvm::ArrayRef<clang::TemplateArgument>> template_args =
      GetTemplateArgs(type);
  for (size_t depth = 0; depth < template_args.size(); depth++) {
    const auto& args_at_depth = template_args[depth];
    for (const clang::TemplateArgument& arg : args_at_depth) {
      if (arg.getKind() == clang::TemplateArgument::Type) {
        if (llvm::Error err = callback(depth, arg.getAsType())) {
          return err;
        }
      } else if (arg.getKind() == clang::TemplateArgument::Pack) {
        for (const clang::TemplateArgument& inner_arg : arg.getPackAsArray()) {
          if (inner_arg.getKind() == clang::TemplateArgument::Type) {
            if (llvm::Error err = callback(depth, inner_arg.getAsType())) {
              return err;
            }
          }
        }
      }
    }
  }
  return llvm::Error::success();
}

}  // namespace

llvm::Expected<ValueLifetimes> ValueLifetimes::Create(
    clang::QualType type, LifetimeFactory lifetime_factory) {
  assert(!type.isNull());
  type = type.IgnoreParens();
  ValueLifetimes ret(type);

  if (const auto* fn = clang::dyn_cast<clang::FunctionProtoType>(type)) {
    // TODO(veluca): this will not correctly handle the distinction between
    // parameter and return lifetimes.
    FunctionLifetimeFactorySingleCallback factory(lifetime_factory);
    FunctionLifetimes fn_lftm;
    if (llvm::Error err = FunctionLifetimes::CreateForFunctionType(fn, factory)
                              .moveInto(fn_lftm)) {
      return std::move(err);
    }
    ret.function_lifetimes_ =
        std::make_unique<FunctionLifetimes>(std::move(fn_lftm));
    return ret;
  }

  for (const auto& lftm_param : GetLifetimeParameters(type)) {
    Lifetime l;
    if (llvm::Error err = lifetime_factory(type, lftm_param).moveInto(l)) {
      return std::move(err);
    }
    ret.lifetime_parameters_by_name_.Add(lftm_param, l);
  }

  // Add implicit lifetime parameters for type template parameters.
  if (llvm::Error err = ForEachTemplateArgument(
          type,
          [&ret, &lifetime_factory](int depth,
                                    clang::QualType arg_type) -> llvm::Error {
            ValueLifetimes template_arg_lifetime;
            if (llvm::Error err =
                    ValueLifetimes::Create(arg_type, lifetime_factory)
                        .moveInto(template_arg_lifetime)) {
              return err;
            }
            if (ret.template_argument_lifetimes_.size() <= depth) {
              ret.template_argument_lifetimes_.resize(depth + 1);
            }
            ret.template_argument_lifetimes_[depth].push_back(
                template_arg_lifetime);
            return llvm::Error::success();
          })) {
    return std::move(err);
  }

  clang::QualType pointee = PointeeType(type);
  if (pointee.isNull()) return ret;
  ObjectLifetimes obj_lftm;
  if (llvm::Error err = ObjectLifetimes::Create(pointee, lifetime_factory)
                            .moveInto(obj_lftm)) {
    return std::move(err);
  }
  ret.pointee_lifetimes_ =
      std::make_unique<ObjectLifetimes>(std::move(obj_lftm));
  return ret;
}

llvm::Expected<ObjectLifetimes> ObjectLifetimes::Create(
    clang::QualType type, LifetimeFactory lifetime_factory) {
  ValueLifetimes v;
  if (llvm::Error err =
          ValueLifetimes::Create(type, lifetime_factory).moveInto(v)) {
    return std::move(err);
  }
  Lifetime l;
  if (llvm::Error err = lifetime_factory(type, "").moveInto(l)) {
    return std::move(err);
  }
  return ObjectLifetimes(l, v);
}

ValueLifetimes ValueLifetimes::ForLifetimeLessType(clang::QualType type) {
  assert(PointeeType(type).isNull() && !type->isRecordType());
  return ValueLifetimes(type);
}

ValueLifetimes ValueLifetimes::ForPointerLikeType(
    clang::QualType type, const ObjectLifetimes& object_lifetimes) {
  assert(!PointeeType(type).isNull());
  ValueLifetimes result(type);
  result.pointee_lifetimes_ =
      std::make_unique<ObjectLifetimes>(object_lifetimes);
  return result;
}

ValueLifetimes ValueLifetimes::ForRecord(
    clang::QualType type,
    std::vector<std::vector<std::optional<ValueLifetimes>>>
        template_argument_lifetimes,
    LifetimeSymbolTable lifetime_parameters) {
  assert(type->isRecordType());
  assert(GetTemplateArgs(type).size() == template_argument_lifetimes.size());
  for (size_t depth = 0; depth < template_argument_lifetimes.size(); ++depth) {
    assert(GetTemplateArgs(type)[depth].size() ==
           template_argument_lifetimes[depth].size());
  }
  ValueLifetimes result(type);
  result.template_argument_lifetimes_ = std::move(template_argument_lifetimes);
  result.lifetime_parameters_by_name_ = lifetime_parameters;
  return result;
}

std::string ValueLifetimes::DebugString(
    const LifetimeFormatter& formatter) const {
  if (!PointeeType(Type()).isNull()) {
    assert(pointee_lifetimes_);
    return pointee_lifetimes_->DebugString(formatter);
  }
  if (clang::isa<clang::FunctionProtoType>(Type())) {
    assert(function_lifetimes_);
    std::string fn_lifetimes = function_lifetimes_->DebugString(formatter);
    if (fn_lifetimes.empty()) return "";
    return absl::StrCat("(", fn_lifetimes, ")");
  }

  std::vector<std::vector<std::string>> tmpl_lifetimes;
  for (auto& tmpl_arg_at_depth : template_argument_lifetimes_) {
    tmpl_lifetimes.emplace_back();
    for (const std::optional<ValueLifetimes>& tmpl_arg : tmpl_arg_at_depth) {
      if (tmpl_arg) {
        std::string inner = tmpl_arg->DebugString(formatter);
        if (!inner.empty()) {
          tmpl_lifetimes.back().push_back(std::move(inner));
        }
      }
    }
  }
  std::vector<std::string> lifetime_parameters;
  for (const auto& lftm_arg : GetLifetimeParameters(type_)) {
    std::optional<Lifetime> lifetime =
        lifetime_parameters_by_name_.LookupName(lftm_arg);
    assert(lifetime.has_value());
    lifetime_parameters.push_back(formatter(*lifetime));
  }
  bool empty_tmpl_lifetimes = true;
  for (const auto& tmpl_arg_at_depth : tmpl_lifetimes) {
    for (const auto& tmpl : tmpl_arg_at_depth) {
      empty_tmpl_lifetimes &= tmpl.empty();
    }
  }
  if (empty_tmpl_lifetimes && lifetime_parameters.empty()) {
    return "";
  } else if (empty_tmpl_lifetimes) {
    if (lifetime_parameters.size() == 1) {
      return lifetime_parameters[0];
    }
    return absl::StrCat("[", absl::StrJoin(lifetime_parameters, ", "), "]");
  } else if (lifetime_parameters.empty() && tmpl_lifetimes.size() == 1 &&
             tmpl_lifetimes[0].size() == 1) {
    return tmpl_lifetimes[0][0];
  }

  std::vector<std::string> tmpl_lifetimes_per_depth;
  tmpl_lifetimes_per_depth.reserve(tmpl_lifetimes.size());
  for (const auto& tmpl_depth : tmpl_lifetimes) {
    tmpl_lifetimes_per_depth.push_back(
        absl::StrCat("<", absl::StrJoin(tmpl_depth, ", "), ">"));
  }
  std::string lifetime_parameter_string;
  if (!lifetime_parameters.empty()) {
    lifetime_parameter_string =
        absl::StrCat(" [", absl::StrJoin(lifetime_parameters, ", "), "]");
  }

  return absl::StrCat(absl::StrJoin(tmpl_lifetimes_per_depth, "::"),
                      lifetime_parameter_string);
}

const ObjectLifetimes& ValueLifetimes::GetPointeeLifetimes() const {
  assert(!PointeeType(type_).isNull());
  return *pointee_lifetimes_;
}

void ValueLifetimes::Traverse(std::function<void(Lifetime&, Variance)> visitor,
                              Variance variance) {
  for (auto& tmpl_arg_at_depth : template_argument_lifetimes_) {
    for (std::optional<ValueLifetimes>& tmpl_arg : tmpl_arg_at_depth) {
      if (tmpl_arg) {
        tmpl_arg->Traverse(visitor, kInvariant);
      }
    }
  }
  if (pointee_lifetimes_) {
    pointee_lifetimes_->Traverse(visitor, variance, Type());
  }
  for (const auto& lftm_arg : GetLifetimeParameters(type_)) {
    std::optional<Lifetime> lifetime =
        lifetime_parameters_by_name_.LookupName(lftm_arg);
    assert(lifetime.has_value());
    Lifetime new_lifetime = *lifetime;
    visitor(new_lifetime, variance);

    // Note that this check is not an optimization, but a guard against UB:
    // if one calls the const version of Traverse, with a callback that does not
    // mutate the lifetime, on a const instance of the Value/ObjectLifetimes,
    // then calling Rebind would be UB, even if Rebind would do nothing in
    // practice.
    if (new_lifetime != lifetime) {
      lifetime_parameters_by_name_.Rebind(lftm_arg, new_lifetime);
    }
  }
  if (function_lifetimes_) {
    function_lifetimes_->Traverse(visitor);
  }
}

void ValueLifetimes::Traverse(
    std::function<void(const Lifetime&, Variance)> visitor,
    Variance variance) const {
  const_cast<ValueLifetimes*>(this)->Traverse(
      [&visitor](Lifetime& l, Variance v) { visitor(l, v); }, variance);
}

ValueLifetimes::ValueLifetimes(clang::QualType type) : type_(type) {}

const llvm::SmallVector<llvm::ArrayRef<clang::TemplateArgument>>
GetTemplateArgs(clang::QualType type) {
  llvm::SmallVector<llvm::ArrayRef<clang::TemplateArgument>> result;

  if (auto elaborated = type->getAs<clang::ElaboratedType>()) {
    if (clang::NestedNameSpecifier* qualifier = elaborated->getQualifier()) {
      if (const clang::Type* qualifier_type = qualifier->getAsType()) {
        result = GetTemplateArgs(clang::QualType(qualifier_type, 0));
      }
    }
  }

  if (auto specialization = type->getAs<clang::TemplateSpecializationType>()) {
    result.push_back(specialization->template_arguments());
  } else if (auto record = type->getAs<clang::RecordType>()) {
    if (auto specialization_decl =
            clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
                record->getDecl())) {
      result.push_back(specialization_decl->getTemplateArgs().asArray());
    }
  }

  return result;
}

ObjectLifetimes ObjectLifetimes::GetObjectLifetimesForTypeInContext(
    clang::QualType type, llvm::SmallVector<std::string> type_lifetime_args,
    llvm::StringRef object_lifetime_parameter) const {
  assert(value_lifetimes_.Type()->isRecordType());

  // The object of the `type` (i.e. field or a base class) basically has the
  // same lifetime as the struct.
  Lifetime ret_lifetime = lifetime_;
  // ... unless the field has lifetime parameters.
  if (!object_lifetime_parameter.empty()) {
    ret_lifetime =
        value_lifetimes_.GetLifetimeParameter(object_lifetime_parameter);
  }

  // `type` is one of a template argument, a struct, a pointer, or a type
  // with no lifetimes (other than its own).

  // First case: template argument. We just attach the
  // template argument's lifetimes to the leaf ObjectLifetimes.
  if (auto targ = type->getAs<clang::SubstTemplateTypeParmType>()) {
    const clang::TemplateTypeParmType* type_parm = targ->getReplacedParameter();
    const std::optional<ValueLifetimes>& arg_lifetimes =
        value_lifetimes_.GetTemplateArgumentLifetimes(type_parm->getDepth(),
                                                      type_parm->getIndex());
    if (!arg_lifetimes) {
      assert(false);
      return llvm::DenseMapInfo<ObjectLifetimes>::getEmptyKey();
    }
    return ObjectLifetimes(ret_lifetime, *arg_lifetimes);
  } else if (type->isStructureOrClassType()) {
    // Second case: struct.
    // Resolve lifetime parameters for the struct, if it has any.
    LifetimeSymbolTable lifetime_params;
    llvm::SmallVector<std::string> params = GetLifetimeParameters(type);
    for (size_t i = params.size(); i-- > 0;) {
      assert(!type_lifetime_args.empty());
      auto lftm_arg = type_lifetime_args.back();
      type_lifetime_args.pop_back();
      lifetime_params.Add(params[i],
                          value_lifetimes_.GetLifetimeParameter(lftm_arg));
    }

    // We need to construct potentally reshuffled
    // template arguments, if the struct is a template.

    // TODO(veluca): mixing lifetimes and template parameters is not supported
    // yet.
    std::vector<std::vector<std::optional<ValueLifetimes>>>
        template_argument_lifetimes;
    for (const auto& args_at_depth : GetTemplateArgs(type)) {
      size_t parameter_pack_expansion_index = 0;
      template_argument_lifetimes.push_back({});
      for (const clang::TemplateArgument& arg : args_at_depth) {
        if (arg.getKind() == clang::TemplateArgument::Type) {
          if (auto templ_arg =
                  clang::dyn_cast<clang::SubstTemplateTypeParmType>(
                      arg.getAsType())) {
            const clang::TemplateTypeParmType* type_parm =
                templ_arg->getReplacedParameter();
            // Template parameter packs get the index of the *pack*, not the
            // index of the type inside the pack itself. As they must appear
            // last, we can just increase the counter at every occurrence and
            // wraparound when we run out of template arguments.
            size_t index = type_parm->getIndex();
            if (type_parm->isParameterPack()) {
              index += parameter_pack_expansion_index++;
              if (index + 1 >= value_lifetimes_.GetNumTemplateArgumentsAtDepth(
                                   type_parm->getDepth())) {
                parameter_pack_expansion_index = 0;
              }
            }
            template_argument_lifetimes.back().push_back(
                value_lifetimes_.GetTemplateArgumentLifetimes(
                    type_parm->getDepth(), index));
          } else {
            // Create a new ValueLifetimes of the type of the template
            // parameter, with lifetime `lifetime_`.
            // TODO(veluca): we need to propagate lifetime parameters here.
            template_argument_lifetimes.back().push_back(
                ValueLifetimes::Create(arg.getAsType(), [this](
                                                            clang::QualType,
                                                            llvm::StringRef) {
                  return this->lifetime_;
                }).get());
          }
        } else {
          template_argument_lifetimes.back().push_back(std::nullopt);
        }
      }
    }

    return ObjectLifetimes(
        ret_lifetime,
        ValueLifetimes::ForRecord(type, std::move(template_argument_lifetimes),
                                  std::move(lifetime_params)));
  } else if (clang::QualType pointee_type = PointeeType(type);
             !pointee_type.isNull()) {
    std::string object_lifetime_parameter;
    if (!type_lifetime_args.empty()) {
      object_lifetime_parameter = std::move(type_lifetime_args.back());
      type_lifetime_args.pop_back();
    }
    // Third case: pointer.
    return ObjectLifetimes(
        ret_lifetime, ValueLifetimes::ForPointerLikeType(
                          type, GetObjectLifetimesForTypeInContext(
                                    pointee_type, std::move(type_lifetime_args),
                                    object_lifetime_parameter)));
  }

  return ObjectLifetimes(ret_lifetime,
                         ValueLifetimes::ForLifetimeLessType(type));
}

std::string ObjectLifetimes::DebugString(
    const LifetimeFormatter& formatter) const {
  std::string inner_lifetimes = value_lifetimes_.DebugString(formatter);
  std::string lifetime = formatter(lifetime_);
  if (inner_lifetimes.empty()) {
    return lifetime;
  }
  return absl::StrCat(std::move(inner_lifetimes), ", ", std::move(lifetime));
}

ObjectLifetimes ObjectLifetimes::GetFieldOrBaseLifetimes(
    clang::QualType type,
    llvm::SmallVector<std::string> type_lifetime_args) const {
  return GetObjectLifetimesForTypeInContext(type, std::move(type_lifetime_args),
                                            "");
}

void ObjectLifetimes::Traverse(std::function<void(Lifetime&, Variance)> visitor,
                               Variance variance,
                               clang::QualType indirection_type) {
  assert(indirection_type.isNull() ||
         indirection_type->getPointeeType().IgnoreParens() == Type());
  value_lifetimes_.Traverse(
      visitor, indirection_type.isNull() || indirection_type.isConstQualified()
                   ? kCovariant
                   : kInvariant);
  visitor(lifetime_, variance);
}

void ObjectLifetimes::Traverse(
    std::function<void(const Lifetime&, Variance)> visitor, Variance variance,
    clang::QualType indirection_type) const {
  const_cast<ObjectLifetimes*>(this)->Traverse(
      [&visitor](Lifetime& l, Variance v) { visitor(l, v); }, variance,
      indirection_type);
}

llvm::Expected<llvm::StringRef> EvaluateAsStringLiteral(
    const clang::Expr* expr, const clang::ASTContext& ast_context) {
  auto error = []() {
    return llvm::createStringError(
        llvm::inconvertibleErrorCode(),
        "cannot evaluate argument as a string literal");
  };

  clang::Expr::EvalResult eval_result;
  if (!expr->EvaluateAsConstantExpr(eval_result, ast_context) ||
      !eval_result.Val.isLValue()) {
    return error();
  }

  const auto* eval_result_expr =
      eval_result.Val.getLValueBase().dyn_cast<const clang::Expr*>();
  if (!eval_result_expr) {
    return error();
  }

  const auto* strlit = clang::dyn_cast<clang::StringLiteral>(eval_result_expr);
  if (!strlit) {
    return error();
  }

  return strlit->getString();
}

}  // namespace devtools_rust

namespace llvm {

bool DenseMapInfo<devtools_rust::ValueLifetimes>::isEqual(
    const devtools_rust::ValueLifetimes& lhs,
    const devtools_rust::ValueLifetimes& rhs) {
  if (lhs.type_ != rhs.type_) {
    return false;
  }
  if ((lhs.pointee_lifetimes_ == nullptr) !=
      (rhs.pointee_lifetimes_ == nullptr)) {
    return false;
  }
  if (lhs.pointee_lifetimes_ &&
      !DenseMapInfo<devtools_rust::ObjectLifetimes>::isEqual(
          *lhs.pointee_lifetimes_, *rhs.pointee_lifetimes_)) {
    return false;
  }
  if (lhs.template_argument_lifetimes_.size() !=
      rhs.template_argument_lifetimes_.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.template_argument_lifetimes_.size(); i++) {
    if (lhs.template_argument_lifetimes_[i].size() !=
        rhs.template_argument_lifetimes_[i].size()) {
      return false;
    }
    for (size_t j = 0; j < lhs.template_argument_lifetimes_[i].size(); j++) {
      const auto& alhs = lhs.template_argument_lifetimes_[i][j];
      const auto& arhs = rhs.template_argument_lifetimes_[i][j];
      if (alhs.has_value() != arhs.has_value()) {
        return false;
      }
      if (alhs.has_value() && !isEqual(*alhs, *arhs)) {
        return false;
      }
    }
  }
  if (lhs.lifetime_parameters_by_name_.GetMapping() !=
      rhs.lifetime_parameters_by_name_.GetMapping()) {
    return false;
  }
  return true;
}

unsigned DenseMapInfo<devtools_rust::ValueLifetimes>::getHashValue(
    const devtools_rust::ValueLifetimes& value_lifetimes) {
  llvm::hash_code hash = 0;
  if (value_lifetimes.pointee_lifetimes_) {
    hash = DenseMapInfo<devtools_rust::ObjectLifetimes>::getHashValue(
        *value_lifetimes.pointee_lifetimes_);
  }
  for (const auto& lifetimes_at_depth :
       value_lifetimes.template_argument_lifetimes_) {
    for (const auto& tmpl_lifetime : lifetimes_at_depth) {
      if (tmpl_lifetime) {
        hash = hash_combine(hash, getHashValue(*tmpl_lifetime));
      }
    }
  }
  for (const auto& lifetime_arg :
       value_lifetimes.lifetime_parameters_by_name_.GetMapping()) {
    hash = hash_combine(hash, DenseMapInfo<llvm::StringRef>::getHashValue(
                                  lifetime_arg.first()));
    hash =
        hash_combine(hash, DenseMapInfo<devtools_rust::Lifetime>::getHashValue(
                               lifetime_arg.second));
  }
  return hash_combine(
      hash, DenseMapInfo<clang::QualType>::getHashValue(value_lifetimes.type_));
}

}  // namespace llvm

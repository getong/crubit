// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef DEVTOOLS_RUST_CC_INTEROP_LIFETIME_ANALYSIS_OBJECT_REPOSITORY_H_
#define DEVTOOLS_RUST_CC_INTEROP_LIFETIME_ANALYSIS_OBJECT_REPOSITORY_H_

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

#include "lifetime_analysis/object.h"
#include "lifetime_analysis/object_set.h"
#include "lifetime_analysis/points_to_map.h"
#include "lifetime_annotations/function_lifetimes.h"
#include "lifetime_annotations/lifetime.h"
#include "lifetime_annotations/type_lifetimes.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/Type.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace tidy {
namespace lifetimes {

// A record-type expression has 2 modes:
// 1. If it's being assigned to a reference, then the contents of the expression
//    are a glvalue. This is because references require an object to point to.
// 2. If it's being assigned to a record object, then the expression itself is
//    not creating an object, but initializing it. So the expression's type is
//    a pure value, and it acts _on_ the initializing object instead of
//    producing an object.
inline bool IsInitExprInitializingARecordObject(const clang::Expr* expr) {
  return expr->getType()->isRecordType() && expr->isPRValue();
}

// Returns the lifetimes of the given FunctionDecl, or an error if they are
// unknown or analysis failed on that FunctionDecl.
FunctionLifetimesOrError GetFunctionLifetimes(
    const FunctionDecl* decl,
    const llvm::DenseMap<const FunctionDecl*, FunctionLifetimesOrError>&
        known_lifetimes);

// A repository for the objects used in the lifetime analysis of a single
// function.
// This class establishes a relationship between AST nodes (e.g. variable
// declarations) and the objects that represent them. It also stores additional
// information about objects that does not change during the analysis.
// The `ObjectRepository` only stores state that does not change during the
// analysis; it is therefore not part of the lattice.
class ObjectRepository {
 private:
  using MapType = llvm::DenseMap<const clang::ValueDecl*, const Object*>;

 public:
  // Tag struct for InitializedObject: the object being initialized is the
  // return value of the function.
  struct ReturnValue {};

  // Maps a given struct-Object to the Object for each of its fields.
  // TODO(veluca): this approach does not produce correct results when
  // diamond-problem-style multiple inheritance happens.
  using FieldObjects =
      llvm::DenseMap<std::pair<const Object*, const clang::FieldDecl*>,
                     const Object*>;

  // Maps a given struct-Object to the Object for each of its bases.
  using BaseObjects =
      llvm::DenseMap<std::pair<const Object*, const clang::Type*>,
                     const Object*>;

  // Iterator refers to a pair consisting of a variable declaration and the
  // object representing that variable.
  using const_iterator = MapType::const_iterator;
  using value_type = MapType::value_type;

  // Creates an ObjectRepository with all relevant objects for the analysis
  // of `func`. This includes:
  // - Variables that are declared or referenced in `func`.
  // - Functions that have their address taken in `func`.
  // - String literals.
  static llvm::Expected<ObjectRepository> Create(
      const clang::FunctionDecl* func,
      const llvm::DenseMap<const clang::FunctionDecl*,
                           FunctionLifetimesOrError>& callee_lifetimes);

  // Move-only.
  ObjectRepository(ObjectRepository&&) = default;
  ObjectRepository& operator=(ObjectRepository&&) = default;

  // Returns a human-readable representation of the mapping.
  std::string DebugString() const;

  const_iterator begin() const { return object_repository_.begin(); }
  const_iterator end() const { return object_repository_.end(); }

  // Returns the object associated with a variable or function.
  const Object* GetDeclObject(const clang::ValueDecl* decl) const;

  // Returns the object associated with a materialize temporary expression.
  const Object* GetTemporaryObject(
      const clang::MaterializeTemporaryExpr* expr) const;

  // Returns the object representing the value of a function parameter at
  // function entry.
  // Note: This `Object` does not represent the parameter variable itself;
  // use GetDeclObject() to retrieve that. We're using an `Object` here
  // because we don't have a dedicated "value" class, but you should not
  // use this object's identity in any way; i.e. no other `Object` in the
  // points-to map should ever point to the object returned by this
  // function.
  const Object* GetOriginalParameterValue(
      const clang::ParmVarDecl* var_decl) const;

  // Synthetizes a FunctionLifetimes that has as lifetimes the initial lifetimes
  // of the function currently being analyzed, before any analysis runs.
  FunctionLifetimes GetOriginalFunctionLifetimes() const;

  // Returns the object associated with an argument to a CallExpr.
  const Object* GetCallExprArgumentObject(const clang::CallExpr* expr,
                                          size_t arg_index) const;

  // Returns the object associated with the return value of a CallExpr.
  const Object* GetCallExprRetObject(const clang::Expr* expr) const;

  // Returns the "virtual" lifetimes for a given function call.
  const FunctionLifetimes& GetCallExprVirtualLifetimes(
      const clang::Expr* expr) const;

  // Returns the object associated with the `this` argument to a CallExpr that
  // represents a method call. Note that this object represents the `this`
  // pointer, not the object that the method is being called on.
  const Object* GetCallExprThisPointer(const clang::CallExpr* expr) const;

  // Returns the object associated with an argument to a CXXConstructExpr.
  const Object* GetCXXConstructExprArgumentObject(
      const clang::CXXConstructExpr* expr, size_t arg_index) const;

  // Returns the object associated with the `this` argument to a
  // CXXConstructExpr. Note that this object represents the `this` pointer, not
  // the object that the method is being called on (which is represented by the
  // object from GetResultObject()).
  const Object* GetCXXConstructExprThisPointer(
      const clang::CXXConstructExpr* expr) const;

  // Returns the object associated with, and initialized by, a constructor call
  // (CXXConstructExpr) or a initializer list (CXXInitListExpr). Note that this
  // represents the actual class object being initialized, not the `this`
  // pointer to it that is passed to methods of the class, and which is
  // represented by the object from GetCXXConstructExprThisPointer().
  const Object* GetResultObject(const clang::Expr* initializer_expr) const;

  // Returns the object that represents `*this`, if in a member function.
  std::optional<const Object*> GetThisObject() const { return this_object_; }

  // Returns the `Object` associated with the return value of the function.
  // Unlike the `Object`s for variables, the "return value object" is a fiction
  // -- there is not, in general, going to be a single object associated with
  // the return value, and it will not, in general, be possible to take the
  // address of the return value object. It's still a useful fiction, however,
  // because it allows us to treat return values the same way as other values.
  const Object* GetReturnObject() const { return return_object_; }

  // Returns an object that represents (the lifetimes of) any string literal.
  const Object* GetStringLiteralObject() const {
    return string_literal_object_;
  }

  // Returns the object associated with a given field in the struct
  // represented by `struct_object`.
  const Object* GetFieldObject(const Object* struct_object,
                               const clang::FieldDecl* field) const;

  // Returns the objects associated with a given field in the structs
  // represented by `struct_objects`.
  ObjectSet GetFieldObject(const ObjectSet& struct_objects,
                           const clang::FieldDecl* field) const;

  // Returns FieldObjects; useful for producing debugging output.
  const FieldObjects& GetFieldObjects() const { return field_object_map_; }

  // Returns the object associated with a given base of the struct
  // represented by `struct_object`.
  const Object* GetBaseClassObject(const Object* struct_object,
                                   const clang::Type* base) const;
  const Object* GetBaseClassObject(const Object* struct_object,
                                   const clang::QualType base) const {
    return GetBaseClassObject(struct_object, base.getTypePtr());
  }

  // Returns the objects associated with a given base of the structs
  // represented by `struct_object`.
  ObjectSet GetBaseClassObject(const ObjectSet& struct_objects,
                               const clang::Type* base) const;
  ObjectSet GetBaseClassObject(const ObjectSet& struct_objects,
                               const clang::QualType base) const {
    return GetBaseClassObject(struct_objects, base.getTypePtr());
  }

  // Returns BaseObjects; useful for producing debugging output.
  const BaseObjects& GetBaseObjects() const { return base_object_map_; }

  // Returns the PointsToMap implied by variable declarations, i.e. assuming
  // that no code has been executed yet.
  const PointsToMap& InitialPointsToMap() const {
    return initial_points_to_map_;
  }

  // Returns the single-valued object set implied by variable declarations, i.e.
  // assuming that no code has been executed yet.
  const ObjectSet& InitialSingleValuedObjects() const {
    return initial_single_valued_objects_;
  }

  // Creates an object (and its related objects, such as objects for its fields,
  // bases or pointees) with the given lifetimes. Points-to relations will be
  // added to the given points_to_map.
  // The returned Object will live as long as this ObjectRepository.
  // TODO(veluca): this is currently public just because of
  // TransferDefaultConstructor in analyze.cc; in principle such objects
  // could/should be created in advance.
  const Object* CreateObject(const ObjectLifetimes& object_lifetimes,
                             PointsToMap& points_to_map);

 private:
  ObjectRepository() = default;

  // Creates an object (and related objects) for a given `type`; lifetimes of
  // created objects are defined by the given
  // `lifetime_factory`/`root_object_lifetime`, and the
  // resulting points-to relations are stored in the ObjectRepository's
  // initial_points_to_map_.
  const Object* CreateObject(clang::QualType type,
                             Lifetime root_object_lifetime,
                             LifetimeFactory lifetime_factory);

  template <typename... Args>
  const Object* ConstructObject(Args&&... args);

  const Object* CloneObject(const Object* object);

  std::optional<const Object*> GetFieldObjectInternal(
      const Object* struct_object, const clang::FieldDecl* field) const;

  // Owns all the `const Object*` members of the object repository.
  llvm::SpecificBumpPtrAllocator<Object> object_allocator_;

  // Map from each variable declaration to the object which it declares.
  MapType object_repository_;

  // Map from each materialized temporary to the object which it declares.
  llvm::DenseMap<const clang::MaterializeTemporaryExpr*, const Object*>
      temporary_objects_;

  // Map from each function parameter to an object representing its initial
  // value at function entry.
  llvm::DenseMap<const clang::ParmVarDecl*, const Object*>
      initial_parameter_object_;

  // Map from each initializer (constructors or initializer lists) to the result
  // object which it initializes.
  //
  // An object in this map may occur in other places too: `object_repository_`
  // if it is an lvalue, or `return_object_`. Or it may be a temporary in which
  // case it is only found in this map.
  llvm::DenseMap<const clang::Expr*, const Object*> result_objects_;

  std::optional<const Object*> this_object_;
  const Object* return_object_;

  ObjectSet initial_single_valued_objects_;

  class VarDeclVisitor;

  PointsToMap initial_points_to_map_;
  FieldObjects field_object_map_;
  BaseObjects base_object_map_;

  llvm::DenseMap<std::pair<const clang::Expr*, size_t>, const Object*>
      call_expr_args_objects_;

  llvm::DenseMap<const clang::Expr*, const Object*> call_expr_this_pointers_;

  llvm::DenseMap<const clang::Expr*, const Object*> call_expr_ret_objects_;

  llvm::DenseMap<const clang::Expr*, FunctionLifetimes>
      call_expr_virtual_lifetimes_;

  const Object* string_literal_object_ = nullptr;

  const clang::FunctionDecl* func_ = nullptr;

  llvm::DenseMap<const Object*, ObjectLifetimes> initial_object_lifetimes_;

  class ObjectCreator;
};

}  // namespace lifetimes
}  // namespace tidy
}  // namespace clang

#endif  // DEVTOOLS_RUST_CC_INTEROP_LIFETIME_ANALYSIS_OBJECT_REPOSITORY_H_

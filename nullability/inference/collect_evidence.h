// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CRUBIT_NULLABILITY_INFERENCE_COLLECT_EVIDENCE_H_
#define CRUBIT_NULLABILITY_INFERENCE_COLLECT_EVIDENCE_H_

#include <memory>
#include <string>
#include <string_view>

#include "absl/base/nullability.h"
#include "absl/container/flat_hash_map.h"
#include "nullability/inference/inference.proto.h"
#include "nullability/inference/slot_fingerprint.h"
#include "nullability/pointer_nullability_analysis.h"
#include "nullability/pragma.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/Analysis/FlowSensitive/Solver.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/FunctionExtras.h"
#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/Support/Error.h"

namespace clang::tidy::nullability {

using USRCache = llvm::DenseMap<const Decl *, std::string>;

std::string_view getOrGenerateUSR(USRCache &Cache, const Decl &);

/// Describes the direction of flow for a piece of evidence between a virtual
/// method and its overrides.
enum class VirtualMethodEvidenceFlowDirection {
  kFromBaseToDerived,
  kFromDerivedToBase,
  kBoth,
};

/// Returns the direction of flow for a piece of evidence between a virtual
/// method and its overrides.
///
/// The direction is determined by whether the evidence points towards Nonnull
/// or Nullable and is for a return slot or a parameter slot.
VirtualMethodEvidenceFlowDirection getFlowDirection(Evidence::Kind Kind,
                                                    bool ForReturnSlot);

// Map from virtual methods in a TU to the set of methods that override them in
// that TU.
using VirtualMethodOverridesMap =
    absl::flat_hash_map<absl::Nonnull<const CXXMethodDecl *>,
                        llvm::DenseSet<absl::Nonnull<const CXXMethodDecl *>>>;
/// Collect a map from virtual methods to a set of their overrides.
VirtualMethodOverridesMap getVirtualMethodOverrides(ASTContext &Ctx);

/// Callback used to report collected nullability evidence.
using EvidenceEmitter = void(const Decl &Target, Slot, Evidence::Kind,
                             SourceLocation);
/// Creates an EvidenceEmitter that serializes the evidence as Evidence protos.
/// This emitter caches USR generation, and should be reused for the whole AST.
/// All parameters must outlive the returned EvidenceEmitter.
llvm::unique_function<EvidenceEmitter> evidenceEmitter(
    llvm::unique_function<void(const Evidence &) const>, USRCache &USRCache,
    ASTContext &Ctx);

/// Creates an EvidenceEmitter as above, but allows re-use of a
/// VirtualMethodOverridesMap if one has already been computed.
llvm::unique_function<EvidenceEmitter> evidenceEmitter(
    llvm::unique_function<void(const Evidence &) const>, USRCache &USRCache,
    ASTContext &Ctx, const VirtualMethodOverridesMap &&OverridesMap);

struct PreviousInferences {
  const llvm::DenseSet<SlotFingerprint> &Nullable = {};
  const llvm::DenseSet<SlotFingerprint> &Nonnull = {};
};

/// Creates a solver with default parameters that is suitable for passing to
/// `collectEvidenceFromDefinition()`.
std::unique_ptr<dataflow::Solver> makeDefaultSolverForInference();

/// Analyze code (such as a function body or variable initializer) to infer
/// nullability.
///
/// Produces Evidence constraining the nullability slots of the symbols that
/// the code interacts with, such as the function's own parameters.
/// This is based on the code's behavior and our definition of null-safety.
///
/// It is up to the caller to ensure the definition is eligible for inference
/// (function has a body, is not dependent, etc).
llvm::Error collectEvidenceFromDefinition(
    const Decl &, llvm::function_ref<EvidenceEmitter>, USRCache &USRCache,
    const NullabilityPragmas &Pragmas,
    PreviousInferences PreviousInferences = {},
    const SolverFactory &MakeSolver = makeDefaultSolverForInference);

/// Gathers evidence of a symbol's nullability from a declaration of it.
///
/// These are trivial "inferences" of what's already written in the code. e.g:
///   void foo(Nullable<int*>);
/// The first parameter of foo must be nullable.
///
/// It is the caller's responsibility to ensure that the symbol is inferable.
void collectEvidenceFromTargetDeclaration(const clang::Decl &,
                                          llvm::function_ref<EvidenceEmitter>,
                                          const NullabilityPragmas &Pragmas);

/// Describes locations within an AST that provide evidence for use in
/// inference.
struct EvidenceSites {
  /// Declarations of inferable symbols.
  llvm::DenseSet<const Decl *> Declarations;
  /// Definitions (e.g. function body, variable initializer) that can be
  /// analyzed.
  /// This will always be concrete code, not a template pattern. These may be
  /// passed to collectEvidenceFromDefinition().
  llvm::DenseSet<const Decl *> Definitions;

  /// Find the evidence sites within the provided AST. If
  /// RestrictToMainFileOrHeader is true, only looks for evidence sites in the
  /// main file or its associated header. Implicit declarations are never
  /// considered to be in the main file or header.
  static EvidenceSites discover(ASTContext &,
                                bool RestrictToMainFileOrHeader = false);
};

/// Returns the slot number for the I'th parameter (0-based).
inline Slot paramSlot(unsigned I) { return static_cast<Slot>(SLOT_PARAM + I); }

}  // namespace clang::tidy::nullability

#endif  // CRUBIT_NULLABILITY_INFERENCE_COLLECT_EVIDENCE_H_

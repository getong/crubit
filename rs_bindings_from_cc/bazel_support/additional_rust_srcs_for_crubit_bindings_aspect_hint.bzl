# Part of the Crubit project, under the Apache License v2.0 with LLVM
# Exceptions. See /LICENSE for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""The `additional_rust_srcs_for_crubit_bindings` aspect hint, when attached to a `cc_library`,
specifies additional Rust source files to be included in the generated Rust crate by Crubit.

WARNING: This presents novel compatibility problems, tread with caution. See below.

Typical uses:

 *  Adding trait implementations to a type defined in C++, which call existing C++ functions and
    methods.
 *  Renaming methods or functions to a more typical Rust name.
 *  Defining completely hand-written ABI-equivalent or automatically-bridged wrapper Rust types
    which substitute for a given C++ type. (Especially to support templates, or where Crubit's
    automatically generated struct is otherwise insufficient or unavailable.)
*   Defining inherently Rust-specific helper types, such as an iterator type for the `IntoIter`
    trait implementation of a C++ container type.

This should **not** be used to introduce substantial new functionality, because any new functions or
types defined in a `additional_rust_srcs_for_crubit_bindings` source file are inaccessible
to any C++ callers. Crubit cannot run on this file in the reverse direction (to generate C++
bindings from Rust source code).

Rust and C++ form one ecosystem: anything a Rust caller may want to do, a C++ caller may want to
do as well. Typically, abstractions should be defined in the C++ file, and exposed to Rust callers
through Crubit.

## Compatibility

SUMMARY: If a newly-introduced item in an additional Rust source file might conflict with a future Crubit
release, then rename the C++ entity using CRUBIT_RUST_NAME.

TODO: b/402478920 - Renamed constructor traits are not callable in any useful way at the moment.

Changes to Crubit can introduce new items into the automatically generated bindings,
which can introduce a conflict when you use `additional_rust_srcs_for_crubit_bindings`.

For example, suppose that Crubit could not generate bindings for a method `void Foo(CustomType x);`,
because `CustomType` was not yet supported. If the `additional_rust_srcs_for_crubit_bindings` source
file defines `impl MyType { pub fn Foo(&self) {} }`, then this will work... as long as Crubit does
not generate bindings for `Foo` in the future.

**Avoid this.** This will block Crubit releases!

Fortunately, Crubit supports a workaround: you can use `CRUBIT_RUST_NAME` to rename the C++ entity
to something else, if it ever receives bindings. Then the original name (or trait), which you
want to implement, cannot conflict with the future Crubit bindings.

"""

load("@bazel_skylib//lib:collections.bzl", "collections")

visibility([
    # <internal link> start
    "//examples/...",
    "//rs_bindings_from_cc/...",
    "//support/...",
    # <internal link> end
])

AdditionalRustSrcsProviderInfo = provider(
    doc = """
The provider that specifies the Rust source files to be included in the Rust crate along with
generated Rust bindings of this C++ target.
""",
    fields = {
        "srcs": "The Rust source files to be included in addition to generated Rust bindings.",
        "namespace_path": "The namespace path for the Rust source files.",
    },
)

def _additional_rust_srcs_for_crubit_bindings_impl(ctx):
    return [AdditionalRustSrcsProviderInfo(
        srcs = ctx.attr.srcs,
        namespace_path = ctx.attr.namespace_path,
    )]

additional_rust_srcs_for_crubit_bindings = rule(
    attrs = {
        "srcs": attr.label_list(
            doc = "The Rust source files to be included in addition to generated Rust bindings.",
            allow_files = True,
            mandatory = True,
        ),
        "namespace_path": attr.string(
            doc = """This allows Rust source files define new entries inside of a specific existing C++ namespace instead of the top level namespace.
For modules which are not existing namespace names, use `pub mod` statement in the Rust source file instead.""",
            mandatory = False,
            default = "",
        ),
    },
    implementation = _additional_rust_srcs_for_crubit_bindings_impl,
    doc = """
Defines an aspect hint that is used to pass extra Rust source files to `rs_bindings_from_cc` tool's
`extra_rs_srcs` CLI argument.

Note: to use `std` in the extra Rust source files, you must use:
```rust
extern crate std;
```
""",
)

def get_additional_rust_srcs(aspect_ctx):
    """Returns `extra_rs_srcs` associated with the `_target`.

    Args:
        aspect_ctx: The ctx from an aspect_hint.

    Returns:
        A list of `File` and its module paths as specified by the `extra_rs_srcs`.
    """
    additional_rust_srcs = []
    for hint in aspect_ctx.rule.attr.aspect_hints:
        if AdditionalRustSrcsProviderInfo in hint:
            ns_path = hint[AdditionalRustSrcsProviderInfo].namespace_path
            for target in hint[AdditionalRustSrcsProviderInfo].srcs:
                srcs = [(f, ns_path) for f in target.files.to_list()]
                additional_rust_srcs.extend(srcs)
    return collections.uniq(additional_rust_srcs)

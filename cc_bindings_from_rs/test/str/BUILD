load(
    "@rules_rust//rust:defs.bzl",
    "rust_library",
)
load(
    "//cc_bindings_from_rs/bazel_support:cc_bindings_from_rust_rule.bzl",
    "cc_bindings_from_rust",
)
load(
    "//cc_bindings_from_rs/test/golden:golden_test.bzl",
    "golden_test",
)
load("//common:crubit_wrapper_macros_oss.bzl", "crubit_cc_test")

package(default_applicable_licenses = ["//:license"])

rust_library(
    name = "str",
    srcs = ["str.rs"],
)

golden_test(
    name = "str_golden_test",
    basename = "str",
    golden_h = "str_cc_api.h",
    golden_rs = "str_cc_api_impl.rs",
    rust_library = "str",
)

cc_bindings_from_rust(
    name = "str_cc_api",
    testonly = 1,
    crate = ":str",
)

crubit_cc_test(
    name = "str_test",
    srcs = ["str_test.cc"],
    deps = [
        ":str_cc_api",
        "@googletest//:gtest_main",
    ],
)

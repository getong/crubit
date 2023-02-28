// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "rs_bindings_from_cc/cmdline.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"
#include "common/status_macros.h"
#include "llvm/Support/JSON.h"

ABSL_FLAG(bool, do_nothing, false,
          "if set to true the tool will produce empty files "
          "(useful for testing Bazel integration)");
ABSL_FLAG(std::string, rs_out, "",
          "output path for the Rust source file with bindings");
ABSL_FLAG(std::string, cc_out, "",
          "output path for the C++ source file with bindings implementation");
ABSL_FLAG(std::string, ir_out, "",
          "(optional) output path for the JSON IR. If not present, the JSON IR "
          "will not be dumped.");
ABSL_FLAG(std::string, crubit_support_path, "",
          "path to the crubit/support directory in a format that "
          "should be used in the #include directives inside the generated .cc "
          "files.");
ABSL_FLAG(std::string, clang_format_exe_path, "",
          "Path to a clang-format executable that will be used to format the "
          ".cc files generated by the tool.");
ABSL_FLAG(std::string, rustfmt_exe_path, "",
          "Path to a rustfmt executable that will be used to format the "
          ".rs files generated by the tool.");
ABSL_FLAG(std::string, rustfmt_config_path, "",
          "(optional) path to a rustfmt.toml file that should replace the "
          "default formatting of the .rs files generated by the tool.");
ABSL_FLAG(std::vector<std::string>, public_headers, std::vector<std::string>(),
          "public headers of the cc_library this tool should generate bindings "
          "for, in a format suitable for usage in google3-relative quote "
          "include (#include \"\").");
ABSL_FLAG(std::string, target, "", "The target to generate bindings for.");
ABSL_FLAG(std::string, target_args, "",
          "Per-target Crubit arguments, encoded as a JSON array. This contains "
          "both the list of headers assigned to the target (h), and the set of "
          "enabled features (f). For example:"
          "[\n"
          "  {\n"
          "     \"t\": \"//foo/bar:baz\",\n"
          "     \"h\": [\"foo/bar/header1.h\", \"foo/bar/header2.h\"],\n"
          "     \"f\": [\"supported\"]\n"
          "  },\n"
          "...\n"
          "]");
// Deprecated alias just for migration purposes.
ABSL_FLAG(std::string, targets_and_headers, "", "DEPRECATED, use target_args")
    .OnUpdate([] {
      absl::SetFlag(&FLAGS_target_args,
                    absl::GetFlag(FLAGS_targets_and_headers));
    });
ABSL_FLAG(std::vector<std::string>, extra_rs_srcs, std::vector<std::string>(),
          "Additional Rust source files to include into the crate.");
ABSL_FLAG(std::vector<std::string>, srcs_to_scan_for_instantiations,
          std::vector<std::string>(),
          "[template instantiation mode only] all Rust source files of a crate "
          "for which we are instantiating templates.");
ABSL_FLAG(std::string, instantiations_out, "",
          "[template instantiation mode only] output path for the JSON file "
          "with mapping from a template instantiation to a generated Rust "
          "struct name. This file is used by cc_template! macro expansion.");
ABSL_FLAG(std::string, namespaces_out, "",
          "(optional) output path for the JSON file containing the target's"
          "namespace hierarchy.");
ABSL_FLAG(std::string, error_report_out, "",
          "(optional) output path for the JSON error report");

namespace crubit {

namespace {

struct TargetArgs {
  std::string target;
  std::vector<std::string> headers;
};

bool fromJSON(const llvm::json::Value& json, TargetArgs& out,
              llvm::json::Path path) {
  llvm::json::ObjectMapper mapper(json, path);
  return mapper && mapper.map("t", out.target) &&
         mapper.mapOptional("h", out.headers);
}

}  // namespace

absl::StatusOr<Cmdline> Cmdline::Create() {
  return CreateFromArgs(
      absl::GetFlag(FLAGS_target), absl::GetFlag(FLAGS_cc_out),
      absl::GetFlag(FLAGS_rs_out), absl::GetFlag(FLAGS_ir_out),
      absl::GetFlag(FLAGS_namespaces_out),
      absl::GetFlag(FLAGS_crubit_support_path),
      absl::GetFlag(FLAGS_clang_format_exe_path),
      absl::GetFlag(FLAGS_rustfmt_exe_path),
      absl::GetFlag(FLAGS_rustfmt_config_path), absl::GetFlag(FLAGS_do_nothing),
      absl::GetFlag(FLAGS_public_headers), absl::GetFlag(FLAGS_target_args),
      absl::GetFlag(FLAGS_extra_rs_srcs),
      absl::GetFlag(FLAGS_srcs_to_scan_for_instantiations),
      absl::GetFlag(FLAGS_instantiations_out),
      absl::GetFlag(FLAGS_error_report_out));
}

absl::StatusOr<Cmdline> Cmdline::CreateFromArgs(
    std::string current_target, std::string cc_out, std::string rs_out,
    std::string ir_out, std::string namespaces_out,
    std::string crubit_support_path, std::string clang_format_exe_path,
    std::string rustfmt_exe_path, std::string rustfmt_config_path,
    bool do_nothing, std::vector<std::string> public_headers,
    std::string target_args_str, std::vector<std::string> extra_rs_srcs,
    std::vector<std::string> srcs_to_scan_for_instantiations,
    std::string instantiations_out, std::string error_report_out) {
  Cmdline cmdline;
  if (current_target.empty()) {
    return absl::InvalidArgumentError("please specify --target");
  }
  cmdline.current_target_ = BazelLabel(std::move(current_target));

  if (rs_out.empty()) {
    return absl::InvalidArgumentError("please specify --rs_out");
  }
  cmdline.rs_out_ = std::move(rs_out);

  if (cc_out.empty()) {
    return absl::InvalidArgumentError("please specify --cc_out");
  }
  cmdline.cc_out_ = std::move(cc_out);

  cmdline.ir_out_ = std::move(ir_out);

  cmdline.namespaces_out_ = std::move(namespaces_out);

  if (crubit_support_path.empty()) {
    return absl::InvalidArgumentError("please specify --crubit_support_path");
  }
  cmdline.crubit_support_path_ = std::move(crubit_support_path);

  if (clang_format_exe_path.empty()) {
    return absl::InvalidArgumentError("please specify --clang_format_exe_path");
  }
  cmdline.clang_format_exe_path_ = std::move(clang_format_exe_path);

  if (rustfmt_exe_path.empty()) {
    return absl::InvalidArgumentError("please specify --rustfmt_exe_path");
  }
  cmdline.rustfmt_exe_path_ = std::move(rustfmt_exe_path);

  cmdline.rustfmt_config_path_ = std::move(rustfmt_config_path);
  cmdline.do_nothing_ = do_nothing;

  if (public_headers.empty()) {
    return absl::InvalidArgumentError("please specify --public_headers");
  }
  std::transform(public_headers.begin(), public_headers.end(),
                 std::back_inserter(cmdline.public_headers_),
                 [](const std::string& s) { return HeaderName(s); });

  cmdline.extra_rs_srcs_ = std::move(extra_rs_srcs);

  if (srcs_to_scan_for_instantiations.empty() != instantiations_out.empty()) {
    return absl::InvalidArgumentError(
        "please specify both --rust_sources and --instantiations_out when "
        "requesting a template instantiation mode");
  }
  cmdline.instantiations_out_ = std::move(instantiations_out);
  cmdline.srcs_to_scan_for_instantiations_ =
      std::move(srcs_to_scan_for_instantiations);
  cmdline.error_report_out_ = std::move(error_report_out);

  if (target_args_str.empty()) {
    return absl::InvalidArgumentError("please specify --target_args");
  }
  auto target_args =
      llvm::json::parse<std::vector<TargetArgs>>(std::move(target_args_str));
  if (auto err = target_args.takeError()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Malformed `--target_args` argument: ", toString(std::move(err))));
  }
  for (const TargetArgs& it : *target_args) {
    const std::string& target = it.target;
    if (target.empty()) {
      return absl::InvalidArgumentError(
          "Expected `t` fields of `--target_args` to be a non-empty "
          "string");
    }
    for (const std::string& header : it.headers) {
      if (header.empty()) {
        return absl::InvalidArgumentError(
            "Expected `h` (header) fields of `--target_args` to be an "
            "array of non-empty strings");
      }
      BazelLabel target_label(target);
      auto [it, inserted] = cmdline.headers_to_targets_.try_emplace(
          HeaderName(header), std::move(target_label));
      if (!inserted) {
        LOG(WARNING) << "The `--target_args` cmdline argument assigns `"
                     << header << "` header to two conflicting targets: `"
                     << target << "` vs `" << it->second.value() << "`";
        // Assign the one that comes first alphabetically, to get a consistent
        // result.
        if (target_label.value() < it->second.value()) {
          it->second = std::move(target_label);
        }
      }
    }
  }

  for (const HeaderName& public_header : cmdline.public_headers_) {
    CRUBIT_RETURN_IF_ERROR(cmdline.FindHeader(public_header).status());
  }

  return cmdline;
}

absl::StatusOr<BazelLabel> Cmdline::FindHeader(const HeaderName& header) const {
  auto it = headers_to_targets_.find(header);
  if (it == headers_to_targets_.end()) {
    return absl::InvalidArgumentError(absl::Substitute(
        "Couldn't find header '$0' in the `headers_to_target` map "
        "derived from the --target_args cmdline argument",
        header.IncludePath()));
  }
  return it->second;
}

Cmdline::Cmdline() = default;

}  // namespace crubit

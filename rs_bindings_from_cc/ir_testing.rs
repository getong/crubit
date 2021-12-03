// Part of the Crubit project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

use anyhow::Result;

use ffi_types::{FfiU8Slice, FfiU8SliceBox};
use ir::{
    self, CcType, Func, FuncParam, Identifier, Item, MappedType, Record, RsType, SpecialMemberFunc,
    IR,
};

/// Generates `IR` from a header containing `header_source`.
pub fn ir_from_cc(header_source: &str) -> Result<IR> {
    ir_from_cc_dependency(header_source, "// empty header")
}

const DEPENDENCY_HEADER_NAME: &str = "test/dependency_header.h";

/// Generates `IR` from a header that depends on another header.
///
/// `header_source` of the header will be updated to contain the `#include` line
/// for the header with `dependency_header_source`. The name of the dependency
/// target is assumed to be `"//test:dependency"`.
pub fn ir_from_cc_dependency(header_source: &str, dependency_header_source: &str) -> Result<IR> {
    extern "C" {
        fn json_from_cc_dependency(
            header_source: FfiU8Slice,
            dependency_header_source: FfiU8Slice,
        ) -> FfiU8SliceBox;
    }

    let header_source_with_include =
        format!("#include \"{}\"\n\n{}", DEPENDENCY_HEADER_NAME, header_source);
    let header_source_with_include_u8 = header_source_with_include.as_bytes();
    let dependency_header_source_u8 = dependency_header_source.as_bytes();
    let json_utf8 = unsafe {
        json_from_cc_dependency(
            FfiU8Slice::from_slice(header_source_with_include_u8),
            FfiU8Slice::from_slice(dependency_header_source_u8),
        )
        .into_boxed_slice()
    };
    ir::deserialize_ir(&*json_utf8)
}

/// Creates an identifier
pub fn ir_id(name: &str) -> Identifier {
    Identifier { identifier: name.to_string() }
}

/// Creates a simple type instance for `int`/`i32`
pub fn ir_int() -> MappedType {
    MappedType {
        rs_type: RsType { name: "i32".to_string().into(), type_args: vec![], decl_id: None },
        cc_type: CcType {
            name: "int".to_string().into(),
            type_args: vec![],
            is_const: false,
            decl_id: None,
        },
    }
}

/// Creates a simple `FuncParam` with a given name and `int`/`i32` type
pub fn ir_int_param(name: &str) -> FuncParam {
    FuncParam { identifier: ir_id(name), type_: ir_int() }
}

/// Creates a simple `Func` with a given name
pub fn ir_func(name: &str) -> Func {
    let ir = ir_from_cc("inline int REPLACEME() {return 0;}").unwrap();
    for item in ir.take_items() {
        if let Item::Func(mut func) = item {
            func.name = ir::UnqualifiedIdentifier::Identifier(ir_id(name));
            return func;
        }
    }
    panic!("Test IR doesn't contain a function");
}

pub fn ir_public_trivial_special() -> SpecialMemberFunc {
    SpecialMemberFunc {
        definition: ir::SpecialMemberDefinition::Trivial,
        access: ir::AccessSpecifier::Public,
    }
}

/// Creates a simple `Item::Record` with a given name.
pub fn ir_record(name: &str) -> Record {
    let ir = ir_from_cc("struct REPLACEME {};").unwrap();
    for item in ir.take_items() {
        if let Item::Record(mut record) = item {
            record.identifier = ir_id(name);
            return record;
        }
    }
    panic!("Test IR doesn't contain a record");
}

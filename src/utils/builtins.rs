use std::collections::HashMap;
use std::sync::LazyLock;
use apica_common::bytecodes::{ApicaBuiltinFunctionBytecode, ApicaSpecificationBytecode, ApicaTypeBytecode};
use apica_common::values::u32::ValueU32;
use apica_common::values::value::Value;
use crate::nodes::literal::NodeLiteral;
use crate::utils::builtin_func_info::{BuiltinFuncInfo, ParameterInfo};
use crate::utils::position::Position;
use crate::utils::token::TokenKind;

pub static APICA_KEYWORDS: LazyLock<HashMap<&'static str, TokenKind>> = LazyLock::new(|| {
    let mut m = HashMap::new();
    m.insert("var", TokenKind::Var);
    m.insert("const", TokenKind::Const);
    m.insert("func", TokenKind::Func);
    m.insert("lambda", TokenKind::Lambda);

    m.insert("if", TokenKind::If);
    m.insert("else", TokenKind::Else);
    m.insert("while", TokenKind::While);

    m.insert("break", TokenKind::Break);
    m.insert("continue", TokenKind::Continue);
    m.insert("return", TokenKind::Return);

    m.insert("true", TokenKind::True);
    m.insert("false", TokenKind::False);
    m.insert("null", TokenKind::Null);

    m.insert("entry", TokenKind::Entrypoint);
    m.insert("specs", TokenKind::Specifications);

    m.insert("global", TokenKind::Global);

    return m;
});

pub static APICA_TYPES: LazyLock<HashMap<&'static str, ApicaTypeBytecode>> = LazyLock::new(|| {
    let mut m = HashMap::new();

    m.insert("any", ApicaTypeBytecode::Any);
    m.insert("i8", ApicaTypeBytecode::I8);
    m.insert("i16", ApicaTypeBytecode::I16);
    m.insert("i32", ApicaTypeBytecode::I32);
    m.insert("i64", ApicaTypeBytecode::I64);
    m.insert("u8", ApicaTypeBytecode::U8);
    m.insert("u16", ApicaTypeBytecode::U16);
    m.insert("u32", ApicaTypeBytecode::U32);
    m.insert("u64", ApicaTypeBytecode::U64);

    m.insert("f32", ApicaTypeBytecode::F32);
    m.insert("f64", ApicaTypeBytecode::F64);

    m.insert("bool", ApicaTypeBytecode::Bool);

    m.insert("char", ApicaTypeBytecode::Char);
    m.insert("string", ApicaTypeBytecode::String);

    m.insert("error", ApicaTypeBytecode::Error);
    m.insert("type", ApicaTypeBytecode::Type);

    return m;
});

pub static APICA_BUILTIN_FUNCTIONS: LazyLock<HashMap<&'static str, BuiltinFuncInfo>> = LazyLock::new(|| {
    let mut m = HashMap::new();

    m.insert("Quit", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::Quit,
        vec![],
        0,
    ));
    
    m.insert("LoadApp", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LoadApp,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::String), None)],
        1,
    ));

    m.insert("LogInfo", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LogInfo,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LognInfo", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LognInfo,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LogSuccess", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LogSuccess,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LognSuccess", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LognSuccess,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LogWarning", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LogWarning,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LognWarning", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LognWarning,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LogError", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LogError,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));
    
    m.insert("LognError", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::LognError,
        vec![ParameterInfo::init(true, None, None)],
        0,
    ));

    m.insert("SetTitle", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::SetTitle,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::String), None)],
        1,
    ));
    
    m.insert("SetResizable", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::SetResizable,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::Bool), None)],
        1,
    ));

    m.insert("IsKeyReleased", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::IsKeyReleased,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::U32), None)],
        1,
    ));
    
    m.insert("IsKeyJustPressed", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::IsKeyJustPressed,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::U32), None)],
        1,
    ));
    
    m.insert("IsKeyPressed", BuiltinFuncInfo::init(
        ApicaBuiltinFunctionBytecode::IsKeyPressed,
        vec![ParameterInfo::init(false, Some(ApicaTypeBytecode::U32), None)],
        1,
    ));

    return m;
});

pub static APICA_BUILTIN_CONSTANTS: LazyLock<HashMap<&'static str, NodeLiteral>> = LazyLock::new(|| {
    let mut m = HashMap::new();

    m.insert("KEY_Q", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(16))));
    m.insert("KEY_W", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(17))));
    m.insert("KEY_E", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(18))));
    m.insert("KEY_R", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(19))));
    m.insert("KEY_T", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(20))));
    m.insert("KEY_Y", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(21))));

    return m;
});

pub static APICA_SPECIFICATIONS: LazyLock<HashMap<&'static str, ApicaSpecificationBytecode>> = LazyLock::new(|| {
    let mut m = HashMap::new();
    
    m.insert("title", ApicaSpecificationBytecode::Title);
    m.insert("id", ApicaSpecificationBytecode::Id);
    m.insert("logger", ApicaSpecificationBytecode::LoggerActivation);
    m.insert("width", ApicaSpecificationBytecode::WindowWidth);
    m.insert("height", ApicaSpecificationBytecode::WindowHeight);
    m.insert("version", ApicaSpecificationBytecode::Version);
    
    return m;
});
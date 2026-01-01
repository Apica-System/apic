use std::collections::HashMap;
use std::sync::LazyLock;
use apica_common::bytecodes::{ApicaBuiltinFunctionBytecode, ApicaTypeBytecode};
use apica_common::values::u32::ValueU32;
use apica_common::values::value::Value;
use crate::nodes::literal::NodeLiteral;
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

pub static APICA_BUILTIN_FUNCTIONS: LazyLock<HashMap<&'static str, ApicaBuiltinFunctionBytecode>> = LazyLock::new(|| {
    let mut m = HashMap::new();

    m.insert("Quit", ApicaBuiltinFunctionBytecode::Quit);
    m.insert("LoadApp", ApicaBuiltinFunctionBytecode::LoadApp);

    m.insert("LogInfo", ApicaBuiltinFunctionBytecode::LogInfo);
    m.insert("LognInfo", ApicaBuiltinFunctionBytecode::LognInfo);
    m.insert("LogSuccess", ApicaBuiltinFunctionBytecode::LogSuccess);
    m.insert("LognSuccess", ApicaBuiltinFunctionBytecode::LognSuccess);
    m.insert("LogWarning", ApicaBuiltinFunctionBytecode::LogWarning);
    m.insert("LognWarning", ApicaBuiltinFunctionBytecode::LognWarning);
    m.insert("LogError", ApicaBuiltinFunctionBytecode::LogError);
    m.insert("LognError", ApicaBuiltinFunctionBytecode::LognError);

    m.insert("SetTitle", ApicaBuiltinFunctionBytecode::SetTitle);
    m.insert("SetResizable", ApicaBuiltinFunctionBytecode::SetResizable);

    m.insert("IsKeyReleased", ApicaBuiltinFunctionBytecode::IsKeyReleased);
    m.insert("IsKeyJustPressed", ApicaBuiltinFunctionBytecode::IsKeyJustPressed);
    m.insert("IsKeyPressed", ApicaBuiltinFunctionBytecode::IsKeyPressed);

    return m;
});

pub static APICA_BUILTIN_CONSTANTS: LazyLock<HashMap<&'static str, NodeLiteral>> = LazyLock::new(|| {
    let mut m = HashMap::new();

    m.insert("KEY_Q", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(16))));
    m.insert("KEY_W", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(17))));
    m.insert("KEY_E", NodeLiteral::init(Position::init(0, 0), Value::U32(ValueU32::init_with(18))));

    return m;
});
use std::fs::{remove_file, File};
use apica_common::bytecodes::{ApicaBytecode, ApicaTypeBytecode};
use apica_common::values::value::Value;
use bitflags::bitflags;
use crate::nodes::_return::NodeReturn;
use crate::nodes::_while::NodeWhile;
use crate::nodes::binary_op::NodeBinaryOp;
use crate::nodes::compound::NodeCompound;
use crate::nodes::entrypoint::NodeEntrypoint;
use crate::nodes::function_call::NodeFunctionCall;
use crate::nodes::global_scope::NodeGlobalScope;
use crate::nodes::if_else::NodeIfElse;
use crate::nodes::literal::NodeLiteral;
use crate::nodes::node::Node;
use crate::nodes::package_call::NodePackageCall;
use crate::nodes::parameter::NodeParameter;
use crate::nodes::parenthesized::NodeParenthesized;
use crate::nodes::ternary_op::NodeTernaryOp;
use crate::nodes::unary_op::NodeUnaryOp;
use crate::nodes::var_const_call::NodeVarConstCall;
use crate::nodes::var_const_decl::NodeVarConstDecl;
use crate::utils::builtins::{APICA_BUILTIN_CONSTANTS, APICA_BUILTIN_FUNCTIONS};
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;
use crate::utils::position::Position;
use crate::utils::token::TokenKind;
use crate::utils::writer::{write_string, write_u32, write_u64, write_u8};

bitflags! {
    #[derive(Copy, Clone)]
    struct EmitterModifier : u8 {
        const None =        0b0000_0000;
        const Builtin =     0b0000_0001;
        const NoCompound =  0b0000_0010;
    }
}

pub struct Emitter<'a> {
    diag_bag: &'a mut DiagnosticBag,
    output_path: &'a String,
    output_file: File,
}

impl<'a> Emitter<'a> {
    pub fn init(diag_bag: &'a mut DiagnosticBag, output_path: &'a String) -> Option<Emitter<'a>> {
        let output_file = File::create(output_path);
        if output_file.is_err() {
            diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Error,
                String::from("EmitterError: Failed to open the output file"),
            ));
            return None;
        }

        Some(Emitter { diag_bag, output_path, output_file: output_file.unwrap() })
    }

    pub fn emit(&mut self, root: &NodeCompound) {
        if !self.diag_bag.has_any_error() {
            for node in root.get_nodes() {
                self.emit_node(node, EmitterModifier::None);
            }
        }

        if self.diag_bag.has_any_error() {
            if remove_file(self.output_path).is_err() {
                self.diag_bag.add(Diagnostic::init_message(
                    DiagnosticKind::Error,
                    String::from("EmitterError: Failed to delete the output APB file. It may be corrupted"),
                ));
            }
        } else {
            write_u64(&mut self.output_file, ApicaBytecode::EndOfFile as u64, &mut self.diag_bag);
            self.diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Success,
                String::from("ApicaCompiler: The source code has been successfully emitted"),
            ));
        }
    }

    fn emit_node(&mut self, node: &Node, mode: EmitterModifier) {
        match node {
            Node::Bad(_) => {},
            Node::EndOfFile(_) => {},
            Node::Compound(compound) => self.emit_compound(compound, mode),

            Node::Entrypoint(entrypoint) => self.emit_entrypoint(entrypoint, mode),
            Node::PackageCall(package_call) => self.emit_package_call(package_call, mode),
            Node::GlobalScope(global_scope) => self.emit_global_scope(global_scope, mode),

            Node::UnaryOp(unary_op) => self.emit_unary_op(unary_op, mode),
            Node::BinaryOp(binary_op) => self.emit_binary_op(binary_op, mode),
            Node::TernaryOp(ternary_op) => self.emit_ternary_op(ternary_op, mode),

            Node::Break(_) => self.emit_break(),
            Node::Continue(_) => self.emit_continue(),
            Node::Return(ret) => self.emit_return(ret, mode),

            Node::Literal(literal) => self.emit_literal(literal, mode),
            Node::Parenthesized(parenthesized) => self.emit_parenthesized(parenthesized, mode),

            Node::VarConstDecl(var_const_decl) => self.emit_var_const_decl(var_const_decl, mode),
            Node::VarConstCall(var_const_call) => self.emit_var_const_call(var_const_call, mode),
            Node::FuncCall(function_call) => self.emit_function_call(function_call, mode),
            Node::Parameter(parameter) => self.emit_parameter(parameter, mode),

            Node::IfElse(if_else) => self.emit_if_else(if_else, mode),
            Node::While(while_node) => self.emit_while(while_node, mode),
        }
    }

    fn emit_compound(&mut self, compound: &NodeCompound, mode: EmitterModifier) {
        if !mode.contains(EmitterModifier::NoCompound) {
            write_u64(&mut self.output_file, ApicaBytecode::Compound as u64, &mut self.diag_bag);
        }

        for node in compound.get_nodes() {
            self.emit_node(node, mode - EmitterModifier::NoCompound);
        }

        write_u64(&mut self.output_file, ApicaBytecode::EndOfBlock as u64, &mut self.diag_bag);
    }

    fn emit_entrypoint(&mut self, entrypoint: &NodeEntrypoint, mode: EmitterModifier) {
        write_u64(&mut self.output_file, ApicaBytecode::Entrypoint as u64, &mut self.diag_bag);
        write_u64(&mut self.output_file, *entrypoint.get_entry() as u64, &mut self.diag_bag);

        self.emit_node(entrypoint.get_body(), mode | EmitterModifier::NoCompound);
    }

    fn emit_package_call(&mut self, package_call: &NodePackageCall, mode: EmitterModifier) {
        if package_call.get_name() == "APICA" {
            self.emit_node(package_call.get_contained(), mode | EmitterModifier::Builtin);
        } else {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("EmitterError: User-defined packages are not yet supported, maybe try APICA"),
                Position::init_from(package_call.get_position()),
            ));
        }
    }

    fn emit_global_scope(&mut self, global_scope: &NodeGlobalScope, mode: EmitterModifier) {
        write_u64(&mut self.output_file, ApicaBytecode::Global as u64, &mut self.diag_bag);
        if let Node::Compound(compound) = global_scope.get_contained() {
            for node in compound.get_nodes() {
                self.emit_node(node, mode);
            }
        } else {
            self.emit_node(global_scope.get_contained(), mode);
        }

        write_u64(&mut self.output_file, ApicaBytecode::EndOfBlock as u64, &mut self.diag_bag);
    }

    fn emit_unary_op(&mut self, unary_op: &NodeUnaryOp, mode: EmitterModifier) {
        match unary_op.get_operator() {
            TokenKind::Bang => write_u64(&mut self.output_file, ApicaBytecode::Not as u64, &mut self.diag_bag),

            _ => {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("EmitterError: An unsupported unary operator was found"),
                    Position::init_from(unary_op.get_position()),
                ));
                return;
            },
        };

        self.emit_node(unary_op.get_operand(), mode);
    }

    fn emit_binary_op(&mut self, binary_op: &NodeBinaryOp, mode: EmitterModifier) {
        match binary_op.get_operator() {
            TokenKind::Plus => write_u64(&mut self.output_file, ApicaBytecode::Add as u64, &mut self.diag_bag),
            TokenKind::PlusPlus => write_u64(&mut self.output_file, ApicaBytecode::Increment as u64, &mut self.diag_bag),
            TokenKind::Minus => write_u64(&mut self.output_file, ApicaBytecode::Subtract as u64, &mut self.diag_bag),
            TokenKind::MinusMinus => write_u64(&mut self.output_file, ApicaBytecode::Decrement as u64, &mut self.diag_bag),
            TokenKind::Equals => write_u64(&mut self.output_file, ApicaBytecode::Assign as u64, &mut self.diag_bag),
            TokenKind::EqualsEquals => write_u64(&mut self.output_file, ApicaBytecode::Equals as u64, &mut self.diag_bag),
            TokenKind::Less => write_u64(&mut self.output_file, ApicaBytecode::LessThan as u64, &mut self.diag_bag),

            _ => {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("EmitterError: An unsupported binary operator was found"),
                    Position::init_from(binary_op.get_position()),
                ));
                return;
            },
        };

        self.emit_node(binary_op.get_left(), mode);
        if let Some(right) = binary_op.get_right() {
            self.emit_node(right, mode);
        }
    }

    fn emit_ternary_op(&mut self, ternary_op: &NodeTernaryOp, mode: EmitterModifier) {
        write_u64(&mut self.output_file, ApicaBytecode::QuestionOperation as u64, &mut self.diag_bag);

        self.emit_node(ternary_op.get_condition(), mode);
        self.emit_node(ternary_op.get_true_expr(), mode);
        self.emit_node(ternary_op.get_false_expr(), mode);
    }

    fn emit_break(&mut self) {
        write_u64(&mut self.output_file, ApicaBytecode::Break as u64, &mut self.diag_bag);
    }

    fn emit_continue(&mut self) {
        write_u64(&mut self.output_file, ApicaBytecode::Continue as u64, &mut self.diag_bag);
    }

    fn emit_return(&mut self, return_node: &NodeReturn, mode: EmitterModifier) {
        if let Some(expression) = return_node.get_expression() {
            write_u64(&mut self.output_file, ApicaBytecode::FilledReturn as u64, &mut self.diag_bag);
            self.emit_node(expression, mode);
        } else {
            write_u64(&mut self.output_file, ApicaBytecode::BlankReturn as u64, &mut self.diag_bag);
        }
    }

    fn emit_literal(&mut self, literal: &NodeLiteral, _: EmitterModifier) {
        write_u64(&mut self.output_file, ApicaBytecode::Literal as u64, &mut self.diag_bag);
        match literal.get_literal() {
            Value::Null(_) => {
                write_u64(&mut self.output_file, ApicaTypeBytecode::Null as u64, &mut self.diag_bag);
            },

            Value::U32(u32) => {
                write_u64(&mut self.output_file, ApicaTypeBytecode::U32 as u64, &mut self.diag_bag);
                write_u32(&mut self.output_file, u32.get_value().unwrap(), &mut self.diag_bag);
            },

            Value::String(string) => {
                write_u64(&mut self.output_file, ApicaTypeBytecode::String as u64, &mut self.diag_bag);
                write_string(&mut self.output_file, string.get_value().as_ref().unwrap(), &mut self.diag_bag);
            },

            Value::Bool(bool) => {
                write_u64(&mut self.output_file, ApicaTypeBytecode::Bool as u64, &mut self.diag_bag);
                write_u8(&mut self.output_file, bool.get_value().unwrap() as u8, &mut self.diag_bag);
            },

            _ => self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("EmitterError: An unsupported literal was found"),
                Position::init_from(literal.get_position()),
            )),
        };
    }

    fn emit_parenthesized(&mut self, parenthesized: &NodeParenthesized, mode: EmitterModifier) {
        self.emit_node(parenthesized.get_contained(), mode);
    }

    fn emit_var_const_decl(&mut self, decl: &NodeVarConstDecl, mode: EmitterModifier) {
        write_u64(&mut self.output_file, if decl.is_constant() { ApicaBytecode::ConstDecl } else { ApicaBytecode::VarDecl } as u64, &mut self.diag_bag);
        write_string(&mut self.output_file, decl.get_name(), &mut self.diag_bag);
        write_u64(&mut self.output_file, *decl.get_value_kind() as u64, &mut self.diag_bag);

        self.emit_node(decl.get_expression(), mode);
    }

    fn emit_var_const_call(&mut self, call: &NodeVarConstCall, mode: EmitterModifier) {
        if mode.contains(EmitterModifier::Builtin) {
            if let Some(builtin_const) = APICA_BUILTIN_CONSTANTS.get(call.get_name().as_str()) {
                self.emit_literal(builtin_const, mode);
            } else {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    format!("EmitterError: Unknown builtin variable/constant -> {}", call.get_name()),
                    Position::init_from(call.get_position()),
                ));   
            }
        } else {
            write_u64(&mut self.output_file, ApicaBytecode::VarConstCall as u64, &mut self.diag_bag);
            write_string(&mut self.output_file, call.get_name(), &mut self.diag_bag);
        }
    }

    fn emit_function_call(&mut self, call: &NodeFunctionCall, mode: EmitterModifier) {
        if mode.contains(EmitterModifier::Builtin) {
            write_u64(&mut self.output_file, ApicaBytecode::BuiltinFuncCall as u64, &mut self.diag_bag);
            if let Some(builtin_func) = APICA_BUILTIN_FUNCTIONS.get(call.get_name().as_str()) {
                let param_numbers = call.get_parameters().len();
                if param_numbers != builtin_func.get_parameters().len() && param_numbers != builtin_func.get_required() {
                    self.diag_bag.add(Diagnostic::init_complete(
                        DiagnosticKind::Error,
                        format!("EmitterError: Incorrect number of parameters for function `{}`", call.get_name()),
                        Position::init_from(call.get_position()),
                    ));
                    return;
                }

                write_u64(&mut self.output_file, builtin_func.get_bytecode() as u64, &mut self.diag_bag);
                for param in call.get_parameters() {
                    self.emit_parameter(param, mode - EmitterModifier::Builtin);
                }
                write_u64(&mut self.output_file, ApicaBytecode::EndOfBlock as u64, &mut self.diag_bag);
            } else {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("EmitterError: Unknown builtin function call"),
                    Position::init_from(call.get_position()),
                ));
                return;
            }
        } else {
            // TODO: handle user-defined function calls
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("EmitterError: User-defined functions are not yet supported"),
                Position::init_from(call.get_position()),
            ));
        }
    }

    fn emit_parameter(&mut self, param: &NodeParameter, mode: EmitterModifier) {
        self.emit_node(param.get_expression(), mode);
    }

    fn emit_if_else(&mut self, if_else: &NodeIfElse, mode: EmitterModifier) {
        if let Some(else_body) = if_else.get_else_body() {
            write_u64(&mut self.output_file, ApicaBytecode::IfElse as u64, &mut self.diag_bag);
            self.emit_node(if_else.get_condition(), mode);
            self.emit_node(if_else.get_if_body(), mode);
            self.emit_node(else_body, mode);
        } else {
            write_u64(&mut self.output_file, ApicaBytecode::If as u64, &mut self.diag_bag);
            self.emit_node(if_else.get_condition(), mode);
            self.emit_node(if_else.get_if_body(), mode);
        }
    }

    fn emit_while(&mut self, while_node: &NodeWhile, mode: EmitterModifier) {
        write_u64(&mut self.output_file, ApicaBytecode::While as u64, &mut self.diag_bag);
        self.emit_node(while_node.get_condition(), mode);
        self.emit_node(while_node.get_body(), mode);
    }
}
use apica_common::bytecodes::ApicaTypeBytecode;
use apica_common::values::_type::get_kind_repr;
use apica_common::values::value::Value;
use crate::nodes::binary_op::NodeBinaryOp;
use crate::nodes::compound::NodeCompound;
use crate::nodes::entrypoint::NodeEntrypoint;
use crate::nodes::function_call::NodeFunctionCall;
use crate::nodes::global_scope::NodeGlobalScope;
use crate::nodes::literal::NodeLiteral;
use crate::nodes::node::Node;
use crate::nodes::package_call::NodePackageCall;
use crate::nodes::parenthesized::NodeParenthesized;
use crate::nodes::ternary_op::NodeTernaryOp;
use crate::nodes::unary_op::NodeUnaryOp;
use crate::nodes::var_const_decl::NodeVarConstDecl;
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;
use crate::utils::position::Position;
use crate::utils::token::TokenKind;

pub struct Optimizer<'a> {
    diag_bag: &'a mut DiagnosticBag,
}

impl<'a> Optimizer<'a> {
    pub fn init(diag_bag: &'a mut DiagnosticBag) -> Optimizer<'a> {
        Optimizer { diag_bag }
    }

    pub fn optimize(&mut self, root: &mut NodeCompound) {
        self.optimize_compound(root);
    }

    fn optimize_node(&mut self, node: &mut Node) -> Option<Node> {
        match node {
            Node::Entrypoint(entrypoint) => self.optimize_entrypoint(entrypoint),
            Node::GlobalScope(global_scope) => self.optimize_global_scope(global_scope),
            Node::Compound(compound) => self.optimize_compound(compound),

            Node::Literal(literal) => self.optimize_literal(literal),
            Node::Parenthesized(parenthesized) => self.optimize_parenthesized(parenthesized),
            Node::UnaryOp(unary_op) => self.optimize_unary_op(unary_op),
            Node::BinaryOp(binary_op) => self.optimize_binary_op(binary_op),
            Node::TernaryOp(ternary_op) => self.optimize_ternary_op(ternary_op),

            Node::VarConstDecl(vc_decl) => self.optimize_var_const_decl(vc_decl),

            Node::PackageCall(package_call) => self.optimize_package_call(package_call),
            Node::FuncCall(func_call) => self.optimize_func_call(func_call),

            _ => None
        }
    }

    fn optimize_entrypoint(&mut self, entrypoint: &mut NodeEntrypoint) -> Option<Node> {
        self.optimize_node(entrypoint.get_mut_body())
    }

    fn optimize_global_scope(&mut self, global_scope: &mut NodeGlobalScope) -> Option<Node> {
        self.optimize_node(global_scope.get_mut_contained())
    }

    fn optimize_compound(&mut self, compound: &mut NodeCompound) -> Option<Node> {
        for node in compound.get_mut_nodes() {
            if let Some(new_node) = self.optimize_node(node) {
                *node = new_node;
            }
        }

        None
    }

    fn optimize_literal(&mut self, literal: &mut NodeLiteral) -> Option<Node> {
        Some(Node::Literal(literal.clone()))
    }

    fn optimize_parenthesized(&mut self, parenthesized: &mut NodeParenthesized) -> Option<Node> {
        println!("Optimizing parenthesized");
        self.optimize_node(parenthesized.get_mut_contained())
    }

    fn optimize_unary_op(&mut self, unary: &mut NodeUnaryOp) -> Option<Node> {
        if let Some(operand) = self.optimize_node(unary.get_mut_operand()) {
            if let Node::Literal(literal) = operand {
                match unary.get_operator() {
                    TokenKind::Bang => {
                        if let Some(result) = literal.get_literal().not() {
                            Some(Node::Literal(NodeLiteral::init(
                                unary.get_position().clone(),
                                result,
                            )))
                        } else {
                            let error = Value::unary_operation_error("!", literal.get_literal().get_type_representation());
                            self.report_error(error, unary.get_position());
                            None
                        }
                    }

                    _ => None,
                }
            } else {
                unary.set_operand(operand);
                None
            }
        } else {
            None
        }
    }

    fn optimize_binary_op(&mut self, binary: &mut NodeBinaryOp) -> Option<Node> {
        let left_optimized = self.optimize_node(binary.get_mut_left());
        let right = if let Some(right_node) = binary.get_mut_right() {
            self.optimize_node(right_node)
        } else { None };

        if let Some(left) = left_optimized {
            match binary.get_operator() {
                TokenKind::PlusPlus => {
                    if let Node::Literal(mut literal) = left {
                        if let Some(result) = literal.get_mut_literal().increment() {
                            Some(Node::Literal(NodeLiteral::init(
                                binary.get_position().clone(),
                                result,
                            )))
                        } else {
                            let error = Value::unary_operation_error("right ++", literal.get_literal().get_type_representation());
                            self.report_error(error, binary.get_position());
                            None
                        }
                    } else {
                        binary.set_left(left);
                        None
                    }
                },

                TokenKind::MinusMinus => {
                    if let Node::Literal(mut literal) = left {
                        if let Some(result) = literal.get_mut_literal().decrement() {
                            Some(Node::Literal(NodeLiteral::init(
                                binary.get_position().clone(),
                                result,
                            )))
                        } else {
                            let error = Value::unary_operation_error("right --", literal.get_literal().get_type_representation());
                            self.report_error(error, binary.get_position());
                            None
                        }
                    } else {
                        binary.set_left(left);
                        None
                    }
                },

                TokenKind::Plus => {
                    binary.set_left(left);
                    if let Some(right_result) = right {
                        binary.set_right(right_result);
                        if let Node::Literal(left_literal) = binary.get_left() && let Node::Literal(right_literal) = binary.get_right().as_ref().unwrap() {
                            if let Some(result) = left_literal.get_literal().add(right_literal.get_literal()) {
                                println!("Optimizing +");
                                Some(Node::Literal(NodeLiteral::init(
                                    binary.get_position().clone(),
                                    result,
                                )))
                            } else {
                                let error = Value::binary_operation_error("+", left_literal.get_literal().get_type_representation(), right_literal.get_literal().get_type_representation());
                                self.report_error(error, binary.get_position());
                                None
                            }
                        } else {
                            None
                        }
                    } else {
                        None
                    }
                },

                TokenKind::As => {
                    binary.set_left(left);
                    if let Some(right) = binary.get_right() && let Node::Typeof(right_result) = right && let Node::Literal(left_literal) = binary.get_left() {
                        if let Some(result) = left_literal.get_literal().convert(right_result.get_value_kind()) {
                            Some(Node::Literal(NodeLiteral::init(
                                binary.get_position().clone(),
                                result,
                            )))
                        } else {
                            let error = Value::binary_operation_error("as", left_literal.get_literal().get_type_representation(), get_kind_repr(&right_result.get_value_kind()));
                            self.report_error(error, binary.get_position());
                            None
                        }
                    } else {
                        None
                    }
                },

                _ => None,
            }
        } else {
            if let Some(right_optimized) = right {
                binary.set_right(right_optimized);
            }
            None
        }
    }

    fn optimize_ternary_op(&mut self, ternary_op: &mut NodeTernaryOp) -> Option<Node> {
        let true_expr = self.optimize_node(ternary_op.get_mut_true_expr());
        let false_expr = self.optimize_node(ternary_op.get_mut_false_expr());
        if let Some(condition) = self.optimize_node(ternary_op.get_mut_condition()) {
            if let Node::Literal(literal) = condition {
                let bool_condition = literal.get_literal().auto_convert(ApicaTypeBytecode::Bool);
                if let Some(condition_result) = bool_condition && let Value::Bool(result) = condition_result {
                    if let Some(value) = result.get_value() && value {
                        if let Some(_) = true_expr {
                            true_expr
                        } else {
                            Some(ternary_op.get_true_expr().clone())
                        }
                    } else {
                        if let Some(_) = false_expr {
                            false_expr
                        } else {
                            Some(ternary_op.get_false_expr().clone())
                        }
                    }
                } else {
                    let error = Value::binary_operation_error("as", literal.get_literal().get_type_representation(), get_kind_repr(&ApicaTypeBytecode::Bool));
                    self.report_error(error, literal.get_position());
                    None
                }
            } else {
                ternary_op.set_condition(condition);
                None
            }
        } else {
            if let Some(new_true_expr) = true_expr {
                ternary_op.set_true_expr(new_true_expr);
            }

            if let Some(new_false_expr) = false_expr {
                ternary_op.set_false_expr(new_false_expr);
            }

            None
        }
    }

    fn optimize_var_const_decl(&mut self, vc_decl: &mut NodeVarConstDecl) -> Option<Node> {
        if let Some(expression) = self.optimize_node(vc_decl.get_mut_expression()) {
            vc_decl.set_expression(expression);
        }

        None
    }

    fn optimize_package_call(&mut self, package_call: &mut NodePackageCall) -> Option<Node> {
        self.optimize_node(package_call.get_mut_contained())
    }

    fn optimize_func_call(&mut self, func_call: &mut NodeFunctionCall) -> Option<Node> {
        for param in func_call.get_mut_parameters() {
            if let Some(new_node) = self.optimize_node(param.get_mut_expression()) {
                param.set_expression(new_node);
            }
        }
        
        None
    }

    fn report_error(&mut self, error: Value, position: &Position) {
        if let Value::Error(verr) = error {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                format!("{}: {}", verr.get_name().as_ref().unwrap(), verr.get_details().as_ref().unwrap()),
                position.clone(),
            ))
        }
    }
}
use crate::nodes::_break::NodeBreak;
use crate::nodes::_continue::NodeContinue;
use crate::nodes::_return::NodeReturn;
use crate::nodes::_while::NodeWhile;
use crate::nodes::bad::NodeBad;
use crate::nodes::binary_op::NodeBinaryOp;
use crate::nodes::compound::NodeCompound;
use crate::nodes::entrypoint::NodeEntrypoint;
use crate::nodes::eof::NodeEndOfFile;
use crate::nodes::function_call::NodeFunctionCall;
use crate::nodes::global_scope::NodeGlobalScope;
use crate::nodes::if_else::NodeIfElse;
use crate::nodes::literal::NodeLiteral;
use crate::nodes::package_call::NodePackageCall;
use crate::nodes::parameter::NodeParameter;
use crate::nodes::parenthesized::NodeParenthesized;
use crate::nodes::ternary_op::NodeTernaryOp;
use crate::nodes::unary_op::NodeUnaryOp;
use crate::nodes::var_const_call::NodeVarConstCall;
use crate::nodes::var_const_decl::NodeVarConstDecl;
use crate::utils::position::Position;

pub enum Node {
    Bad(NodeBad),
    EndOfFile(NodeEndOfFile),
    Compound(NodeCompound),

    Entrypoint(Box<NodeEntrypoint>),
    PackageCall(Box<NodePackageCall>),
    GlobalScope(Box<NodeGlobalScope>),

    UnaryOp(Box<NodeUnaryOp>),
    BinaryOp(Box<NodeBinaryOp>),
    TernaryOp(Box<NodeTernaryOp>),
    
    Break(NodeBreak),
    Continue(NodeContinue),
    Return(Box<NodeReturn>),
    
    Literal(NodeLiteral),
    Parenthesized(Box<NodeParenthesized>),
    
    VarConstDecl(Box<NodeVarConstDecl>),
    VarConstCall(NodeVarConstCall),
    FuncCall(NodeFunctionCall),
    Parameter(Box<NodeParameter>),
    
    IfElse(Box<NodeIfElse>),
    While(Box<NodeWhile>),
}

impl Node {
    pub fn get_position(&self) -> &Position {
        match self {
            Node::Bad(bad) => bad.get_position(),
            Node::EndOfFile(end_of_file) => end_of_file.get_position(),
            Node::Compound(compound) => compound.get_position(),
            
            Node::Entrypoint(entrypoint) => entrypoint.get_position(),
            Node::PackageCall(package_call) => package_call.get_position(),
            Node::GlobalScope(global_scope) => global_scope.get_position(),
            
            Node::UnaryOp(unary_op) => unary_op.get_position(),
            Node::BinaryOp(binary_op) => binary_op.get_position(),
            Node::TernaryOp(ternary_op) => ternary_op.get_position(),
            
            Node::Break(break_node) => break_node.get_position(),
            Node::Continue(continue_node) => continue_node.get_position(),
            Node::Return(return_node) => return_node.get_position(),
            
            Node::Literal(literal) => literal.get_position(),
            Node::Parenthesized(parenthesized) => parenthesized.get_position(),
            
            Node::VarConstDecl(var_const_decl) => var_const_decl.get_position(),
            Node::VarConstCall(var_const_call) => var_const_call.get_position(),
            Node::FuncCall(func_call) => func_call.get_position(),
            Node::Parameter(parameter) => parameter.get_position(),
            
            Node::IfElse(if_else) => if_else.get_position(),
            Node::While(while_) => while_.get_position(),
        }
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        match self {
            Node::Bad(bad) => bad.show(indent, end),
            Node::EndOfFile(end_of_file) => end_of_file.show(indent, end),
            Node::Compound(compound) => compound.show(indent, end),
            
            Node::Entrypoint(entrypoint) => entrypoint.show(indent, end),
            Node::PackageCall(package_call) => package_call.show(indent, end),
            Node::GlobalScope(global_scope) => global_scope.show(indent, end),
            
            Node::UnaryOp(unary_op) => unary_op.show(indent, end),
            Node::BinaryOp(binary_op) => binary_op.show(indent, end),
            Node::TernaryOp(ternary_op) => ternary_op.show(indent, end),
            
            Node::Break(break_node) => break_node.show(indent, end),
            Node::Continue(continue_node) => continue_node.show(indent, end),
            Node::Return(return_node) => return_node.show(indent, end),
            
            Node::Literal(literal) => literal.show(indent, end),
            Node::Parenthesized(parenthesized) => parenthesized.show(indent, end),
            
            Node::VarConstDecl(var_const_decl) => var_const_decl.show(indent, end),
            Node::VarConstCall(var_const_call) => var_const_call.show(indent, end),
            Node::FuncCall(func_call) => func_call.show(indent, end),
            Node::Parameter(parameter) => parameter.show(indent, end),
            
            Node::IfElse(if_else) => if_else.show(indent, end),
            Node::While(while_) => while_.show(indent, end),
        }
    }
}
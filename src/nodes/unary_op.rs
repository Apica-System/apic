use crate::nodes::node::Node;
use crate::utils::position::Position;
use crate::utils::token::TokenKind;

#[derive(Clone)]
pub struct NodeUnaryOp {
    position: Position,
    operator: TokenKind,
    operand: Node,
}

impl NodeUnaryOp {
    pub fn init(position: Position, operator: TokenKind, operand: Node) -> NodeUnaryOp {
        NodeUnaryOp { position, operator, operand, }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeUnaryOp(op: {:?}){end}", self.operator);

        indent.push(' ');
        indent.push(' ');
        self.operand.show(indent, end);
        indent.pop();
        indent.pop();
    }

    pub fn get_operator(&self) -> TokenKind {
        self.operator
    }

    pub fn get_operand(&self) -> &Node {
        &self.operand
    }

    pub fn get_mut_operand(&mut self) -> &mut Node {
        &mut self.operand
    }

    pub fn set_operand(&mut self, new_node: Node) {
        self.operand = new_node;
    }
}
use crate::nodes::node::Node;
use crate::utils::position::Position;
use crate::utils::token::TokenKind;

pub struct NodeBinaryOp {
    position: Position,
    operator: TokenKind,
    left: Node,
    right: Option<Node>,
}

impl NodeBinaryOp {
    pub fn init(position: Position, operator: TokenKind, left: Node, right: Option<Node>) -> NodeBinaryOp {
        NodeBinaryOp { position, operator, left, right, }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeBinaryOp(op: {:?}){end}", self.operator);

        indent.push(' ');
        indent.push(' ');
        self.left.show(indent, end);
        if let Some(right) = &self.right {
            right.show(indent, end);
        }
        
        indent.pop();
        indent.pop();
    }
    
    pub fn get_operator(&self) -> TokenKind {
        self.operator
    }
    
    pub fn get_left(&self) -> &Node {
        &self.left
    }
    
    pub fn get_right(&self) -> &Option<Node> {
        &self.right
    }
}
use crate::nodes::node::Node;
use crate::utils::position::Position;

pub struct NodeParameter {
    position: Position,
    expression: Node,
}

impl NodeParameter {
    pub fn init(position: Position, expression: Node) -> NodeParameter {
        NodeParameter { position, expression }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeParameter{end}");

        indent.push(' ');
        indent.push(' ');
        self.expression.show(indent, end);
        indent.pop();
        indent.pop();
    }
    
    pub fn get_expression(&self) -> &Node {
        &self.expression
    }
}
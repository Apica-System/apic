use crate::nodes::node::Node;
use crate::utils::position::Position;

pub struct NodeReturn {
    position: Position,
    expression: Option<Node>,
}

impl NodeReturn {
    pub fn init(position: Position, expression: Option<Node>) -> NodeReturn {
        NodeReturn { position, expression }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeReturn{end}");
        if let Some(expression) = &self.expression {
            indent.push(' ');
            indent.push(' ');
            expression.show(indent, end);
            indent.pop();
            indent.pop();
        }
    }
    
    pub fn get_expression(&self) -> &Option<Node> {
        &self.expression
    }
}
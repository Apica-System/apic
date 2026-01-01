use crate::nodes::node::Node;
use crate::utils::position::Position;

pub struct NodeIfElse {
    position: Position,
    condition: Node,
    if_body: Node,
    else_body: Option<Node>,
}

impl NodeIfElse {
    pub fn init(position: Position, condition: Node, if_body: Node, else_body: Option<Node>) -> NodeIfElse {
        NodeIfElse { position, condition, if_body, else_body }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeIfElse{end}");

        indent.push(' ');
        indent.push(' ');
        self.condition.show(indent, end);
        self.if_body.show(indent, end);
        if let Some(else_body) = &self.else_body {
            else_body.show(indent, end);
        }
        
        indent.pop();
        indent.pop();
    }
    
    pub fn get_condition(&self) -> &Node {
        &self.condition
    }
    
    pub fn get_if_body(&self) -> &Node {
        &self.if_body
    }
    
    pub fn get_else_body(&self) -> &Option<Node> {
        &self.else_body
    }
}
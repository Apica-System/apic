use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeWhile {
    position: Position,
    condition: Node,
    body: Node,
}

impl NodeWhile {
    pub fn init(position: Position, condition: Node, body: Node) -> NodeWhile {
        NodeWhile { position, condition, body }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeWhile{end}");

        indent.push(' ');
        indent.push(' ');
        self.condition.show(indent, end);
        self.body.show(indent, end);
        indent.pop();
        indent.pop();
    }

    pub fn get_condition(&self) -> &Node {
        &self.condition
    }

    pub fn get_body(&self) -> &Node {
        &self.body
    }
}
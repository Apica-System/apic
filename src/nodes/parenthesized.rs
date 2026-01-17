use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeParenthesized {
    position: Position,
    contained: Node,
}

impl NodeParenthesized {
    pub fn init(position: Position, contained: Node) -> NodeParenthesized {
        NodeParenthesized { position, contained }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeParenthesized{end}");

        indent.push(' ');
        indent.push(' ');
        self.contained.show(indent, end);
        indent.pop();
        indent.pop();
    }
    
    pub fn get_contained(&self) -> &Node {
        &self.contained
    }

    pub fn get_mut_contained(&mut self) -> &mut Node {
        &mut self.contained
    }
}
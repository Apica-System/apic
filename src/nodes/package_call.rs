use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodePackageCall {
    position: Position,
    name: String,
    contained: Node,
}

impl NodePackageCall {
    pub fn init(position: Position, name: String, contained: Node) -> NodePackageCall {
        NodePackageCall { position, name, contained }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodePackageCall(name: {}){end}", self.name);

        indent.push(' ');
        indent.push(' ');
        self.contained.show(indent, end);
        indent.pop();
        indent.pop();
    }
    
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    pub fn get_contained(&self) -> &Node {
        &self.contained
    }

    pub fn get_mut_contained(&mut self) -> &mut Node {
        &mut self.contained
    }
}
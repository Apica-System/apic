use crate::nodes::node::Node;
use crate::utils::position::Position;

pub struct NodeDataSpecifications {
    position: Position,
    nodes: Vec<Node>,
}

impl NodeDataSpecifications {
    pub fn init(position: Position, nodes: Vec<Node>) -> NodeDataSpecifications {
        NodeDataSpecifications { position, nodes }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeDataSpecifications{end}");
        
        indent.push(' ');
        indent.push(' ');
        for node in &self.nodes {
            node.show(indent, end);
        }
        
        indent.pop();
        indent.pop();
    }
    
    pub fn get_nodes(&self) -> &Vec<Node> {
        &self.nodes
    }
}
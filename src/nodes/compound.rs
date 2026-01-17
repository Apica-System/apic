use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeCompound {
    position: Position,
    nodes: Vec<Node>,
}

impl NodeCompound {
    pub fn init(position: Position, nodes: Vec<Node>) -> NodeCompound {
        NodeCompound { position, nodes }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        if indent.is_empty() {
            print!("ROOT{end}");
        } else {
            print!("{indent}NodeCompound{end}");
        }

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

    pub fn get_mut_nodes(&mut self) -> &mut Vec<Node> {
        &mut self.nodes
    }
}
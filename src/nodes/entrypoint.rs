use crate::nodes::node::Node;
use crate::utils::position::Position;
use apica_common::bytecodes::ApicaEntrypointBytecode;

pub struct NodeEntrypoint {
    position: Position,
    entry: ApicaEntrypointBytecode,
    body: Node,
}

impl NodeEntrypoint {
    pub fn init(position: Position, entry: ApicaEntrypointBytecode, body: Node) -> NodeEntrypoint {
        NodeEntrypoint { position, entry, body }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeEntrypoint(entry: {:?}){end}", self.entry);

        indent.push(' ');
        indent.push(' ');
        self.body.show(indent, end);
        indent.pop();
        indent.pop();
    }
    
    pub fn get_entry(&self) -> &ApicaEntrypointBytecode {
        &self.entry
    }
    
    pub fn get_body(&self) -> &Node {
        &self.body
    }
    
    pub fn get_mut_body(&mut self) -> &mut Node {
        &mut self.body
    }
}
use apica_common::bytecodes::ApicaSpecificationBytecode;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeDataString {
    position: Position,
    bytecode: ApicaSpecificationBytecode,
    value: String,
}

impl NodeDataString {
    pub fn init(position: Position, bytecode: ApicaSpecificationBytecode, value: String) -> NodeDataString {
        NodeDataString { position, bytecode, value }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeDataString(data: {:?}, value: {}){end}", self.bytecode, self.value);
    }
    
    pub fn get_bytecode(&self) -> ApicaSpecificationBytecode {
        self.bytecode
    }
    
    pub fn get_value(&self) -> &String {
        &self.value
    }
}
use apica_common::bytecodes::ApicaSpecificationBytecode;
use crate::utils::position::Position;

pub struct NodeDataBool {
    position: Position,
    bytecode: ApicaSpecificationBytecode,
    value: bool,
}

impl NodeDataBool {
    pub fn init(position: Position, bytecode: ApicaSpecificationBytecode, value: bool) -> NodeDataBool {
        NodeDataBool { position, bytecode, value }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeDataBool(data: {:?}, value: {}){end}", self.bytecode, self.value);
    }
    
    pub fn get_bytecode(&self) -> ApicaSpecificationBytecode {
        self.bytecode
    }
    
    pub fn get_value(&self) -> bool {
        self.value
    }
}
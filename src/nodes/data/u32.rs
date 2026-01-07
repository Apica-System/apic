use apica_common::bytecodes::ApicaSpecificationBytecode;
use crate::utils::position::Position;

pub struct NodeDataU32 {
    position: Position,
    bytecode: ApicaSpecificationBytecode,
    value: u32,
}

impl NodeDataU32 {
    pub fn init(position: Position, bytecode: ApicaSpecificationBytecode, value: u32) -> NodeDataU32 {
        NodeDataU32 { position, bytecode, value }
    }

    pub fn get_position(&self) -> &Position {
        &self.position
    }

    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeDataU32(data: {:?}, value: {}){end}", self.bytecode, self.value);
    }

    pub fn get_bytecode(&self) -> ApicaSpecificationBytecode {
        self.bytecode
    }

    pub fn get_value(&self) -> u32 {
        self.value
    }
}
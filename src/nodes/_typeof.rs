use apica_common::bytecodes::ApicaTypeBytecode;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeTypeof {
    position: Position,
    value_kind: ApicaTypeBytecode,
}

impl NodeTypeof {
    pub fn init(position: Position, value_kind: ApicaTypeBytecode) -> NodeTypeof {
        NodeTypeof { position, value_kind }
    }

    pub fn get_position(&self) -> &Position {
        &self.position
    }

    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeTypeof(type: {:?}){end}", self.value_kind);
    }
    
    pub fn get_value_kind(&self) -> ApicaTypeBytecode {
        self.value_kind
    }
}
use apica_common::values::value::Value;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeLiteral {
    position: Position,
    literal: Value,
}

impl NodeLiteral {
    pub fn init(position: Position, literal: Value) -> NodeLiteral {
        NodeLiteral { position, literal }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeLiteral(val: ");
        self.literal.show('\0');
        print!("){end}");
    }

    pub fn get_literal(&self) -> &Value {
        &self.literal
    }
    
    pub fn get_mut_literal(&mut self) -> &mut Value {
        &mut self.literal
    }
}
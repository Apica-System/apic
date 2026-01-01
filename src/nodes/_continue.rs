use crate::utils::position::Position;

pub struct NodeContinue {
    position: Position,
}

impl NodeContinue {
    pub fn init(position: Position) -> NodeContinue {
        NodeContinue { position }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeContinue{end}");
    }
}
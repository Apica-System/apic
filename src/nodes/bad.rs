use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeBad {
    position: Position,
}

impl NodeBad {
    pub fn init(position: Position) -> NodeBad {
        NodeBad { position }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeBad{end}");
    }
}
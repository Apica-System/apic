use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeBreak {
    position: Position,
}

impl NodeBreak {
    pub fn init(position: Position) -> NodeBreak {
        NodeBreak { position }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeBreak{end}");
    }
}
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeEndOfFile {
    position: Position,
}

impl NodeEndOfFile {
    pub fn init(position: Position) -> NodeEndOfFile {
        NodeEndOfFile { position }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeEndOfFile{end}");
    }
}
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeVarConstCall {
    position: Position,
    name: String,
}

impl NodeVarConstCall {
    pub fn init(position: Position, name: String) -> NodeVarConstCall {
        NodeVarConstCall { position, name }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeVarConstCall(name: {}){end}", self.name);
    }

    pub fn get_name(&self) -> &String {
        &self.name
    }
}
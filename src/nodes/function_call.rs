use crate::nodes::parameter::NodeParameter;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeFunctionCall {
    position: Position,
    name: String,
    parameters: Vec<NodeParameter>,
}

impl NodeFunctionCall {
    pub fn init(position: Position, name: String, parameters: Vec<NodeParameter>) -> NodeFunctionCall {
        NodeFunctionCall { position, name, parameters }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeFunctionCall(name: {}){end}", self.name);

        indent.push(' ');
        indent.push(' ');
        for param in &self.parameters {
            param.show(indent, end);
        }
        
        indent.pop();
        indent.pop();
    }

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn get_parameters(&self) -> &Vec<NodeParameter> {
        &self.parameters
    }
    
    pub fn get_mut_parameters(&mut self) -> &mut Vec<NodeParameter> {
        &mut self.parameters
    }
}
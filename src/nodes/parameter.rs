use crate::nodes::node::Node;
use crate::utils::position::Position;

pub struct NodeParameter {
    position: Position,
    explicit_name: Option<String>,
    expression: Node,
}

impl NodeParameter {
    pub fn init(position: Position, explicit_name: Option<String>, expression: Node) -> NodeParameter {
        NodeParameter { position, explicit_name, expression }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        if let Some(name) = &self.explicit_name {
            print!("{indent}NodeParameter(name: {name}){end}");
        } else {
            print!("{indent}NodeParameter(name: ?){end}");
        }

        indent.push(' ');
        indent.push(' ');
        self.expression.show(indent, end);
        indent.pop();
        indent.pop();
    }
    
    pub fn get_explicit_name(&self) -> &Option<String> {
        &self.explicit_name
    }
    
    pub fn get_expression(&self) -> &Node {
        &self.expression
    }
}
use apica_common::bytecodes::ApicaTypeBytecode;
use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeVarConstDecl {
    position: Position,
    name: String,
    is_constant: bool,
    value_kind: ApicaTypeBytecode,
    expression: Node,
}

impl NodeVarConstDecl {
    pub fn init(position: Position, name: String, is_constant: bool, value_kind: ApicaTypeBytecode, expression: Node) -> NodeVarConstDecl {
        NodeVarConstDecl { position, name, is_constant, value_kind, expression }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeVarConstDecl(name: {}, const: {}, type: {:?}){end}", self.name, self.is_constant, self.value_kind);

        indent.push(' ');
        indent.push(' ');
        self.expression.show(indent, end);
        indent.pop();
        indent.pop();
    }

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn is_constant(&self) -> bool {
        self.is_constant
    }

    pub fn get_value_kind(&self) -> &ApicaTypeBytecode {
        &self.value_kind
    }

    pub fn get_expression(&self) -> &Node {
        &self.expression
    }

    pub fn get_mut_expression(&mut self) -> &mut Node {
        &mut self.expression
    }

    pub fn set_expression(&mut self, new_expression: Node) {
        self.expression = new_expression;
    }
}
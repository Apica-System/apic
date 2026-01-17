use crate::nodes::node::Node;
use crate::utils::position::Position;

#[derive(Clone)]
pub struct NodeTernaryOp {
    position: Position,
    condition: Node,
    true_expr: Node,
    false_expr: Node,
}

impl NodeTernaryOp {
    pub fn init(position: Position, condition: Node, true_expr: Node, false_expr: Node) -> NodeTernaryOp {
        NodeTernaryOp { position, condition, true_expr, false_expr }
    }
    
    pub fn get_position(&self) -> &Position {
        &self.position
    }
    
    pub fn show(&self, indent: &mut String, end: char) {
        print!("{indent}NodeTernaryOp{end}");

        indent.push(' ');
        indent.push(' ');
        self.condition.show(indent, end);
        self.true_expr.show(indent, end);
        self.false_expr.show(indent, end);
        
        indent.pop();
        indent.pop();
    }

    pub fn get_condition(&self) -> &Node {
        &self.condition
    }

    pub fn get_mut_condition(&mut self) -> &mut Node {
        &mut self.condition
    }
    
    pub fn set_condition(&mut self, new_condition: Node) {
        self.condition = new_condition;
    }
    
    pub fn get_true_expr(&self) -> &Node {
        &self.true_expr
    }
    
    pub fn get_mut_true_expr(&mut self) -> &mut Node {
        &mut self.true_expr
    }
    
    pub fn set_true_expr(&mut self, new_true_expr: Node) {
        self.true_expr = new_true_expr;
    }

    pub fn get_false_expr(&self) -> &Node {
        &self.false_expr
    }

    pub fn get_mut_false_expr(&mut self) -> &mut Node {
        &mut self.false_expr
    }
    
    pub fn set_false_expr(&mut self, new_false_expr: Node) {
        self.false_expr = new_false_expr;
    }
}
use crate::nodes::node::Node;
use crate::utils::position::Position;

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

    pub fn get_true_expr(&self) -> &Node {
        &self.true_expr
    }

    pub fn get_false_expr(&self) -> &Node {
        &self.false_expr
    }
}
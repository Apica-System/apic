#include "nodes/unary.hpp"

namespace nodes {
    NodeUnary::NodeUnary(Node *operand, utils::TokenKind _operator) {
        this->operand = operand;
        this->_operator = _operator;
    }

    NodeUnary::~NodeUnary() {
        if (this->operand) delete this->operand;
    }

    const NodeKind NodeUnary::GetKind() const {
        return NodeKind::ND_Unary;
    }

    void NodeUnary::Show(apic_string indent) {
        printf("%sNodeUnary(op: %u)\n", indent.c_str(), this->_operator);
        
        indent.append("  ");
        this->operand->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }
    
    const Node *NodeUnary::GetOperand() const {
        return this->operand;
    }

    const utils::TokenKind NodeUnary::GetOperator() const {
        return this->_operator;
    }
}
#include "nodes/binary.hpp"

namespace nodes {
    NodeBinary::NodeBinary(Node *left, Node *right, utils::TokenKind _operator) {
        this->left = left;
        this->right = right;
        this->_operator = _operator;
    }
    
    NodeBinary::~NodeBinary() {
        if (this->left) delete this->left;
        if (this->right) delete this->right;
    }

    const NodeKind NodeBinary::GetKind() const {
        return NodeKind::ND_Binary;
    }
    
    void NodeBinary::Show(apic_string indent) {
        printf("%sNodeBinary(op: %u)\n", indent.c_str(), this->_operator);
        indent.append("  ");
        this->left->Show(indent);

        if (this->right)
            this->right->Show(indent);
        
        indent.pop_back();
        indent.pop_back();
    }
    
    const Node *NodeBinary::Getleft() const {
        return this->left;
    }

    const Node *NodeBinary::GetRight() const {
        return this->right;
    }
    
    const utils::TokenKind NodeBinary::GetOperator() const {
        return this->_operator;
    }
}
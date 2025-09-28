#include "nodes/return.hpp"

namespace nodes {
    NodeReturn::NodeReturn(Node *expression) {
        this->expression = expression;
    }

    NodeReturn::~NodeReturn() {
        if (this->expression) delete this->expression;
    }

    const NodeKind NodeReturn::GetKind() const {
        return NodeKind::ND_Return;
    }

    void NodeReturn::Show(apic_string indent) {
        printf("%sNodeReturn\n", indent.c_str());
        if (this->expression) {
            indent.append("  ");
            this->expression->Show(indent);
            indent.pop_back();
            indent.pop_back();
        }
    }
    
    const Node *NodeReturn::GetExpression() const {
        return this->expression;
    }
}
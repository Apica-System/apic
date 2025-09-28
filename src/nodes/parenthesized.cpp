#include "nodes/parenthesized.hpp"

namespace nodes {
    NodeParenthesized::NodeParenthesized(Node *expression) {
        this->expression = expression;
    }

    NodeParenthesized::~NodeParenthesized() {
        if (this->expression) delete this->expression;
    }

    const NodeKind NodeParenthesized::GetKind() const {
        return NodeKind::ND_Parenthesized;
    }

    void NodeParenthesized::Show(apic_string indent) {
        printf("%sNodeParenthesized\n", indent.c_str());
        indent.append("  ");

        this->expression->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }

    const nodes::Node *NodeParenthesized::GetExpression() const {
        return this->expression;
    }
}
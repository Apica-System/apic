#include "nodes/literal.hpp"

namespace nodes {
    NodeLiteral::NodeLiteral(values::Value *value) {
        this->value = value;
    }

    NodeLiteral::~NodeLiteral() {
        if (this->value) delete this->value;
    }

    const NodeKind NodeLiteral::GetKind() const {
        return NodeKind::ND_Literal;
    }

    void NodeLiteral::Show(apic_string indent) {
        printf("%sNodeLiteral(type: %u)\n", indent.c_str(), this->value->GetKind());
    }

    const values::Value *NodeLiteral::GetValue() const {
        return this->value;
    }
}
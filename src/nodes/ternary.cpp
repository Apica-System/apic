#include "nodes/ternary.hpp"

namespace nodes {
    NodeTernary::NodeTernary(Node *question, Node *first, Node *second) {
        this->question = question;
        this->first = first;
        this->second = second;
    }

    NodeTernary::~NodeTernary() {
        if (this->question) delete this->question;
        if (this->first) delete this->first;
        if (this->second) delete this->second;
    }

    const NodeKind NodeTernary::GetKind() const {
        return NodeKind::ND_Ternary;
    }

    void NodeTernary::Show(apic_string indent) {
        printf("%sNodeTernary\n", indent.c_str());
        indent.append("  ");

        this->question->Show(indent);
        this->first->Show(indent);
        this->second->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }

    const Node *NodeTernary::GetQuestion() const {
        return this->question;
    }

    const Node *NodeTernary::GetFirst() const {
        return this->first;
    }

    const Node *NodeTernary::GetSecond() const {
        return this->second;
    }
}
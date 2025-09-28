#include "nodes/compound.hpp"

namespace nodes {
    NodeCompound::NodeCompound(const std::vector<Node*> &nodes)
        : nodes(nodes) {

    }

    NodeCompound::~NodeCompound() {
        for (Node *node : this->nodes)
            delete node;
    }

    const NodeKind NodeCompound::GetKind() const {
        return NodeKind::ND_Compound;
    }

    void NodeCompound::Show(apic_string indent) {
        if (indent.empty())
            printf("ROOT\n");
        else
            printf("%sNodeCompound\n", indent.c_str());
        
        indent.append("  ");
        for (Node *node : this->nodes)
            node->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }

    std::vector<Node*> NodeCompound::GetNodes() const {
        return this->nodes;
    }
}
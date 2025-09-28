#include "nodes/entrypoint.hpp"

namespace nodes {
    NodeEntrypoint::NodeEntrypoint(ApicaEntrypointBytecode entrypoint, Node *body) {
        this->entrypoint = entrypoint;
        this->body = body;
    }

    NodeEntrypoint::~NodeEntrypoint() {
        if (this->body) delete this->body;
    }

    const NodeKind NodeEntrypoint::GetKind() const {
        return NodeKind::ND_Entrypoint;
    }

    void NodeEntrypoint::Show(apic_string indent) {
        printf("%sNodeEntrypoint(kind: %llu)\n", indent.c_str(), this->entrypoint);

        indent.append("  ");
        this->body->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }

    const ApicaEntrypointBytecode NodeEntrypoint::GetEntrypoint() const {
        return this->entrypoint;
    }

    Node *NodeEntrypoint::GetBody() const {
        return this->body;
    }
}
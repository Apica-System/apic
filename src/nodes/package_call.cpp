#include "nodes/package_call.hpp"

namespace nodes {
    NodePackageCall::NodePackageCall(const apic_string &name, Node *next)
        : name(name) {
        this->next = next;
    }

    NodePackageCall::~NodePackageCall() {

    }

    const NodeKind NodePackageCall::GetKind() const {
        return NodeKind::ND_PackageCall;
    }

    void NodePackageCall::Show(apic_string indent) {
        printf("%sNodePackageCall(name: %s)\n", indent.c_str(), this->name.c_str());
        indent.append("  ");

        this->next->Show(indent);
        indent.pop_back();
        indent.pop_back();
    }

    apic_string NodePackageCall::GetName() const {
        return this->name;
    }

    Node *NodePackageCall::GetNext() const {
        return this->next;
    }
}
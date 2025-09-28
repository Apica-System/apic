#include "nodes/continue.hpp"

namespace nodes {
    NodeContinue::NodeContinue() {

    }
    
    NodeContinue::~NodeContinue() {

    }

    const NodeKind NodeContinue::GetKind() const {
        return NodeKind::ND_Continue;
    }

    void NodeContinue::Show(apic_string indent) {
        printf("%sNodeContinue\n", indent.c_str());
    }
}
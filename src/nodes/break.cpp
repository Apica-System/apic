#include "nodes/break.hpp"

namespace nodes {
    NodeBreak::NodeBreak() {

    }

    NodeBreak::~NodeBreak() {

    }

    const NodeKind NodeBreak::GetKind() const {
        return NodeKind::ND_Break;
    }
    
    void NodeBreak::Show(apic_string indent) {
        printf("%sNodeBreak\n", indent.c_str());
    }
}
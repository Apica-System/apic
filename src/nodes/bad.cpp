#include "nodes/bad.hpp"

namespace nodes {
    NodeBad::NodeBad() {

    }

    NodeBad::~NodeBad() {

    }

    const NodeKind NodeBad::GetKind() const {
        return NodeKind::ND_Bad;
    }

    void NodeBad::Show(apic_string indent) {
        printf("%sNodeBad\n", indent.c_str());
    }
}
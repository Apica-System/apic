#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeBad : public Node {
    public:
        NodeBad();
        ~NodeBad();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
    };
}
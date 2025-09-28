#pragma once

#include "nodes/break.hpp"

namespace nodes {
    class NodeContinue : public Node {
    public:
        NodeContinue();
        ~NodeContinue();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
    };
}
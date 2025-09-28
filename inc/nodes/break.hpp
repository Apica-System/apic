#pragma once

#include "nodes/literal.hpp"

namespace nodes {
    class NodeBreak : public Node {
    public:
        NodeBreak();
        ~NodeBreak();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
    };
}
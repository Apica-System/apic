#pragma once

#include "nodes/compound.hpp"

namespace nodes {
    class NodeEndOfFile : public Node {
    public:
        NodeEndOfFile();
        ~NodeEndOfFile();        

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
    };
}
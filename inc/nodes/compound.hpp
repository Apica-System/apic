#pragma once

#include "nodes/bad.hpp"

namespace nodes {
    class NodeCompound : public Node {
    public:
        NodeCompound(const std::vector<Node*> &nodes);
        ~NodeCompound();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;

        std::vector<Node*> GetNodes() const;
    private:
        std::vector<Node*> nodes;
    };
}
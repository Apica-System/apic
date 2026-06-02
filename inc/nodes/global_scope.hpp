#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeGlobalScope final : public Node {
    public:
        NodeGlobalScope(const utils::Position &position, Node *contained);
        ~NodeGlobalScope();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        Node *contained;
    };
}
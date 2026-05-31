#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeContinue final : public Node {
    public:
        NodeContinue(const utils::Position &position);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    };
}
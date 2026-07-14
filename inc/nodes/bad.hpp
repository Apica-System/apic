#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeBad final : public Node {
    public:
        NodeBad(const utils::Position &position);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;
    };
}
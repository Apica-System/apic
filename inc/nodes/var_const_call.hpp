#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeVarConstCall final : public Node {
    public:
        NodeVarConstCall(const utils::Position &position, const std::string &name);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;
    private:
        std::string name;
        uint64_t id;
    };
}
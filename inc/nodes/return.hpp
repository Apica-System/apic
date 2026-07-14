#pragma once

#include "nodes/node.hpp"
#include <optional>

namespace nodes {
    class NodeReturn final : public Node {
    public:
        NodeReturn(const utils::Position &position);
        NodeReturn(const utils::Position &position, Node *expr);
        ~NodeReturn();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;
    private:
        std::optional<Node*> returned_expression;
    };
}
#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeFor final : public Node {
    public:
        NodeFor(const utils::Position &position, std::optional<Node*> init, Node *cnd, std::optional<Node*> end, Node *body);
        ~NodeFor();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;
    private:
        std::optional<Node*> initialize;
        Node *condition;
        std::optional<Node*> end;
        Node *body;
    };
}
#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeTernaryOperation final : public Node {
    public:
        NodeTernaryOperation(const utils::Position &position, Node *cnd, Node *true_expr, Node *false_expr);
        ~NodeTernaryOperation();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        Node *condition, *true_expr, *false_expr;
    };
}
#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeWhile final : public Node {
    public:
        NodeWhile(const utils::Position &position, Node *cnd, Node *body);
        ~NodeWhile();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        Node *condition, *body;
    };
}
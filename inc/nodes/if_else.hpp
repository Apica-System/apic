#pragma once

#include "nodes/node.hpp"
#include <optional>

namespace nodes {
    class NodeIfElse final : public Node {
    public:
        NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body);
        NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body, Node *else_body);
        ~NodeIfElse();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        Node *if_condition, *if_body;
        std::optional<Node*> else_body;
    };
}
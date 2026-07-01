#pragma once

#include "nodes/node.hpp"
#include "utils/token.hpp"
#include <optional>

namespace nodes {
    class NodeBinaryOperation final : public Node {
    public:
        NodeBinaryOperation(const utils::Position &position, utils::TokenKind op, Node *left);
        NodeBinaryOperation(const utils::Position &position, utils::TokenKind op, Node *left, Node *right);
        ~NodeBinaryOperation();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        utils::TokenKind binary_operator;
        Node *left;
        std::optional<Node*> right;

        std::optional<nodes::Node*> optimizeFullBinary();
    };
}
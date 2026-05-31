#pragma once

#include "nodes/node.hpp"
#include "utils/token.hpp"

namespace nodes {
    class NodeUnaryOperation final : public Node {
    public:
        NodeUnaryOperation(const utils::Position &position, utils::TokenKind op, Node *operand);
        ~NodeUnaryOperation();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        utils::TokenKind getUnaryOperator() const;
        Node *getOperand() const;
    private:
        utils::TokenKind unary_operator;
        Node *operand;
    };
}
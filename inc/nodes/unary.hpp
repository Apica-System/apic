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
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;
    private:
        utils::TokenKind unary_operator;
        Node *operand;

        utils::OptimizedResult optimizeOperation();
        void checkCorrectVarConstAccess(const utils::OptimizedResult &optimized_operand);
    };
}
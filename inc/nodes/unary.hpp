#pragma once

#include "nodes/return.hpp"

namespace nodes {
    class NodeUnary : public Node {
    public:
        NodeUnary(Node *operand, utils::TokenKind _operator);
        ~NodeUnary();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const Node *GetOperand() const;
        const utils::TokenKind GetOperator() const;
    private:
        Node *operand;
        utils::TokenKind _operator;
    };
}
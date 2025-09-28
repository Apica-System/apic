#pragma once

#include "nodes/unary.hpp"

namespace nodes {
    class NodeBinary : public Node {
    public:
        NodeBinary(Node *left, Node *right, utils::TokenKind _operator);
        ~NodeBinary();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const Node *Getleft() const;
        const Node *GetRight() const;
        const utils::TokenKind GetOperator() const;
    private:
        Node *left, *right;
        utils::TokenKind _operator;
    };
}
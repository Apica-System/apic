#pragma once

#include "nodes/binary.hpp"

namespace nodes {
    class NodeParenthesized : public Node {
    public:
        NodeParenthesized(Node *expression);
        ~NodeParenthesized();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const nodes::Node *GetExpression() const;
    private:
        Node *expression;
    };
}
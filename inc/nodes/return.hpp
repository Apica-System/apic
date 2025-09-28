#pragma once

#include "nodes/continue.hpp"

namespace nodes {
    class NodeReturn : public Node {
    public:
        NodeReturn(Node *expression);
        ~NodeReturn();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const Node *GetExpression() const;
    private:
        Node *expression;
    };
}
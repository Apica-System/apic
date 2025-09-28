#pragma once

#include "nodes/parenthesized.hpp"

namespace nodes {
    class NodeTernary : public Node {
    public:
        NodeTernary(Node *question, Node *first, Node *second);
        ~NodeTernary();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const Node *GetQuestion() const;
        const Node *GetFirst() const;
        const Node *GetSecond() const;
    private:
        Node *question, *first, *second;
    };
}
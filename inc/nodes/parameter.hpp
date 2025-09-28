#pragma once

#include "nodes/package_call.hpp"

namespace nodes {
    class NodeParameter : public Node {
    public:
        NodeParameter(apic_string *name, Node *expr);
        NodeParameter(Node *expr);
        ~NodeParameter();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const apic_string *GetExplicitName() const;
        Node *GetExpression() const;
    private:
        apic_string *explicit_name;
        Node *expression;
    };
}
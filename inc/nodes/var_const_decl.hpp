#pragma once

#include "nodes/func_call.hpp"

namespace nodes {
    class NodeVarConstDecl : public Node {
    public:
        NodeVarConstDecl(const apic_string &name, values::ValueKind vkind, Node *expr);
        ~NodeVarConstDecl();

        apic_string GetName() const;
        const values::ValueKind GetVKind() const;
        Node *GetExpression() const;
    private:
        apic_string name;
        values::ValueKind vkind;
        Node *expression;
    };
}
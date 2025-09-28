#pragma once

#include "nodes/entrypoint.hpp"

namespace nodes {
    class NodePackageCall : public Node {
    public:
        NodePackageCall(const apic_string &name, Node *next);
        ~NodePackageCall();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        apic_string GetName() const;
        Node *GetNext() const;
    private:
        apic_string name;
        Node *next;
    };
}
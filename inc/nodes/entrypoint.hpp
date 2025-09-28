#pragma once

#include "nodes/ternary.hpp"

namespace nodes {
    class NodeEntrypoint : public Node {
    public:
        NodeEntrypoint(ApicaEntrypointBytecode entrypoint, Node *body);
        ~NodeEntrypoint();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const ApicaEntrypointBytecode GetEntrypoint() const;
        Node *GetBody() const;
    private:
        ApicaEntrypointBytecode entrypoint;
        Node *body;
    };
}
#pragma once

#include "nodes/eof.hpp"

namespace nodes {
    class NodeLiteral : public Node {
    public:
        NodeLiteral(values::Value *value);
        ~NodeLiteral();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        const values::Value *GetValue() const;
    private:
        values::Value *value;
    };
}
#pragma once

#include "nodes/parameter.hpp"

namespace nodes {
    class NodeFuncCall : public Node {
    public:
        NodeFuncCall(const apic_string &name, utils::Position *position, const std::vector<NodeParameter*> &parameters);
        ~NodeFuncCall();

        const NodeKind GetKind() const override;
        void Show(apic_string indent) override;
        apic_string GetName() const;
        utils::Position *GetPosition() const;
        std::vector<NodeParameter*> GetParameters() const;
    private:
        utils::Position *position;
        apic_string name;
        std::vector<NodeParameter*> parameters;
    };
}
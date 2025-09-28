#include "nodes/func_call.hpp"

namespace nodes {
    NodeFuncCall::NodeFuncCall(const apic_string &name, utils::Position *position, const std::vector<NodeParameter*> &parameters)
        : name(name), parameters(parameters) {
        this->position = position;
    }

    NodeFuncCall::~NodeFuncCall() {
        if (this->position) delete this->position;
    }

    const NodeKind NodeFuncCall::GetKind() const {
        return NodeKind::ND_FuncCall;
    }

    void NodeFuncCall::Show(apic_string indent) {
        printf("%sNodeFuncCall(name: %s)\n", indent.c_str(), this->name.c_str());
        indent.append("  ");

        for (NodeParameter *param : this->parameters)
            param->Show(indent);
        
        indent.pop_back();
        indent.pop_back();
    }

    apic_string NodeFuncCall::GetName() const {
        return this->name;
    }

    utils::Position *NodeFuncCall::GetPosition() const {
        return this->position;
    }

    std::vector<NodeParameter*> NodeFuncCall::GetParameters() const {
        return this->parameters;
    }
}
#pragma once

#include "nodes/parameter.hpp"
#include <vector>

namespace nodes {
    class NodeFuncCall final : public Node {
    public:
        NodeFuncCall(const utils::Position &position, const std::string &name, const std::vector<NodeParameter*> &params);
        ~NodeFuncCall();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        std::string getFunctionName() const;
        const std::vector<NodeParameter*> getFunctionParameters() const;
    private:
        std::string function_name;
        std::vector<NodeParameter*> parameters;
    };
}
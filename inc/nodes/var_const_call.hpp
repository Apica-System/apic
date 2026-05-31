#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodeVarConstCall final : public Node {
    public:
        NodeVarConstCall(const utils::Position &position, const std::string &name);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        std::string getVarConstName() const;

        uint64_t getDeclarationId() const;
    private:
        std::string name;
        uint64_t id;
    };
}
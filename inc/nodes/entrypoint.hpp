#pragma once

#include "nodes/compound.hpp"
#include "bytecodes.hpp"

namespace nodes {
    class NodeEntrypoint final : public Node {
    public:
        NodeEntrypoint(const utils::Position &position, common::bytecodes::ApicaEntrypointBytecode entry, Node *body);
        ~NodeEntrypoint();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        common::bytecodes::ApicaEntrypointBytecode getEntryBytecode() const;
        Node *getBody() const;
    private:
        common::bytecodes::ApicaEntrypointBytecode entry_bytecode;
        Node *body;
    };
}
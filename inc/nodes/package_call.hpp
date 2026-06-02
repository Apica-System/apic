#pragma once

#include "nodes/node.hpp"

namespace nodes {
    class NodePackageCall final : public Node {
    public:
        NodePackageCall(const utils::Position &position, const std::string &name, Node *contained);
        ~NodePackageCall();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        std::string package_name;
        Node *contained;
    };
}
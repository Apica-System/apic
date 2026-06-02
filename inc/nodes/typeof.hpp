#pragma once

#include "nodes/node.hpp"
#include "values/type.hpp"

namespace nodes {
    class NodeTypeof final : public Node {
    public:
        NodeTypeof(const utils::Position &position, const std::string &vtype);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    private:
        std::string value_type;
    };
}
#pragma once

#include "nodes/node.hpp"
#include "values/value.hpp"

namespace nodes {
    class NodeLiteral final : public Node {
    public:
        NodeLiteral(common::values::Value *value);
        NodeLiteral(const utils::Position &position, common::values::Value *value);
        ~NodeLiteral();

        static NodeLiteral *copyBuiltin(const NodeLiteral &builtin);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        common::values::Value *getValue() const;
    private:
        common::values::Value *value;
        bool is_copy;
    };
}
#pragma once

#include "nodes/node.hpp"
#include <optional>

namespace nodes {
    class NodeParameter final : public Node {
    public:
        NodeParameter(const utils::Position &position, Node *expression);
        NodeParameter(const utils::Position &position, Node *expression, const std::string &name);
        ~NodeParameter();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        Node *getParameterExpression() const;
        std::optional<std::string> getParameterName() const;
    private:
        Node *expression;
        std::optional<std::string> parameter_name;
    };
}
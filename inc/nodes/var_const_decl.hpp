#pragma once

#include "nodes/node.hpp"
#include "values/type.hpp"
#include "bytecodes.hpp"

namespace nodes {
    class NodeVarConstDeclaration final : public Node {
    public:
        NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, const std::string &vtype);
        NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, Node *expression);
        NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, const std::string &vtype, Node *expression);
        ~NodeVarConstDeclaration();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;

        std::string getVarConstName() const;
        bool isConstant() const;
        std::optional<std::string> getVarConstType() const;
        std::optional<Node*> getInitExpression() const;

        uint64_t getDeclarationId() const;
        void setDeclarationId(uint64_t id);
    private:
        std::string name;
        uint64_t id;
        bool is_constant;
        std::optional<std::string> value_type;
        std::optional<Node*> expression;
    };
}
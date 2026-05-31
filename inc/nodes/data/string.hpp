#pragma once

#include "nodes/data/data.hpp"

namespace nodes {
    class NodeDataString final : public NodeData {
    public:
        NodeDataString(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, const std::string &value);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;

        std::string getSpecificationValue() const;
    private:
        std::string value;
    };
}
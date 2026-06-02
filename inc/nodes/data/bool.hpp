#pragma once

#include "nodes/data/data.hpp"

namespace nodes {
    class NodeDataBool final : public NodeData {
    public:
        NodeDataBool(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, bool value);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
    private:
        bool value;
    };
}
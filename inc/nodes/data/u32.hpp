#pragma once

#include "nodes/data/data.hpp"

namespace nodes {
    class NodeDataU32 final : public NodeData {
    public:
        NodeDataU32(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, uint32_t value);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
    private:
        uint32_t value;
    };
}
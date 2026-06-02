#pragma once

#include "nodes/data/data.hpp"

namespace nodes {
    class NodeDataU64 final : public NodeData {
    public:
        NodeDataU64(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, uint64_t value);

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
    private:
        uint64_t value;
    };
}
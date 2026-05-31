#include "nodes/data/u64.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeDataU64::NodeDataU64(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, uint64_t value)
    : NodeData(position, spec_bytecode), value(value) {

}

void NodeDataU64::show(std::string &indent, char end) const {
    std::cout << indent << "NodeDataU32(spec: " << this->spec_bytecode << ", val: " << this->value << ')' << end;
}

NodeKind NodeDataU64::getKind() const {
    return NodeKind::DataU32;
}

void NodeDataU64::emit(core::Emitter &emitter) const {
    emitter.writeU64(this->spec_bytecode);
    emitter.writeU64(this->value);
}

uint64_t NodeDataU64::getSpecificationValue() const {
    return this->value;
}
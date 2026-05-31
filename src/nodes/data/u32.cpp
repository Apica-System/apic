#include "nodes/data/u32.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeDataU32::NodeDataU32(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, uint32_t value)
    : NodeData(position, spec_bytecode), value(value) {

}

void NodeDataU32::show(std::string &indent, char end) const {
    std::cout << indent << "NodeDataU32(spec: " << this->spec_bytecode << ", val: " << this->value << ')' << end;
}

NodeKind NodeDataU32::getKind() const {
    return NodeKind::DataU32;
}

void NodeDataU32::emit(core::Emitter &emitter) const {
    emitter.writeU64(this->spec_bytecode);
    emitter.writeU32(this->value);
}

uint32_t NodeDataU32::getSpecificationValue() const {
    return this->value;
}
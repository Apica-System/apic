#include "nodes/data/string.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeDataString::NodeDataString(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, const std::string &value)
    : NodeData(position, spec_bytecode), value(value) {

}

void NodeDataString::show(std::string &indent, char end) const {
    std::cout << indent << "NodeDataString(spec: " << this->spec_bytecode << ", val: " << this->value << ')' << end;
}

NodeKind NodeDataString::getKind() const {
    return NodeKind::DataString;
}

void NodeDataString::emit(core::Emitter &emitter) const {
    emitter.writeU64(this->spec_bytecode);
    emitter.writeString(this->value);
}

std::string NodeDataString::getSpecificationValue() const {
    return this->value;
}
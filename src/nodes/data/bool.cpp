#include "nodes/data/bool.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeDataBool::NodeDataBool(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode, bool value)
    : NodeData(position, spec_bytecode), value(value) {

}

void NodeDataBool::show(std::string &indent, char end) const {
    std::cout << indent << "NodeDataBool(spec: " << this->spec_bytecode << ", val: " << this->value << ')' << end;
}

NodeKind NodeDataBool::getKind() const {
    return NodeKind::DataBool;
}

void NodeDataBool::emit(core::Emitter &emitter) const {
    emitter.writeU64(this->spec_bytecode);
    emitter.writeU8(this->value);
}

bool NodeDataBool::getSpecificationValue() const {
    return this->value;
}
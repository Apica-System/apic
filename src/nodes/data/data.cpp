#include "nodes/data/data.hpp"

using namespace nodes;

NodeData::NodeData(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode)
    : Node(position), spec_bytecode(spec_bytecode) {

}

common::bytecodes::ApicaSpecificationBytecode NodeData::getSpecificationBytecode() const {
    return this->spec_bytecode;
}

void NodeData::setId() {
    
}

utils::OptimizedResult NodeData::optimize(core::Optimizer &) {
    return utils::OptimizedResult(utils::OptimizedFlag::None);
}
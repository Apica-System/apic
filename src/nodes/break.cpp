#include "nodes/break.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeBreak::NodeBreak(const utils::Position &position)
    : Node(position) {

}

void NodeBreak::show(std::string &indent, char end) const {
    std::cout << indent << "NodeBreak" << end;
}

NodeKind NodeBreak::getKind() const {
    return NodeKind::Break;
}

void NodeBreak::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::Break);
}

void NodeBreak::setId() {
    
}

utils::OptimizedResult NodeBreak::optimize(core::Optimizer &) {
    return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
}
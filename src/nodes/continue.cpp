#include "nodes/continue.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeContinue::NodeContinue(const utils::Position &position)
    : Node(position) {

}

void NodeContinue::show(std::string &indent, char end) const {
    std::cout << indent << "NodeContinue" << end;
}

NodeKind NodeContinue::getKind() const {
    return NodeKind::Continue;
}

void NodeContinue::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::Continue);
}

void NodeContinue::setId() {
    
}

utils::OptimizedResult NodeContinue::optimize(core::Optimizer &) {
    return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
}
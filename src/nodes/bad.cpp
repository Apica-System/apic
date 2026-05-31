#include "nodes/bad.hpp"
#include <iostream>

using namespace nodes;

NodeBad::NodeBad(const utils::Position &position)
    : Node(position) {

}

void NodeBad::show(std::string &indent, char end) const {
    std::cout << indent << "NodeBad" << end;
}

NodeKind NodeBad::getKind() const {
    return NodeKind::Bad;
}

void NodeBad::emit(core::Emitter &) const {
    
}

void NodeBad::setId() {
    
}

std::optional<nodes::Node*> NodeBad::optimize(core::Optimizer &) {
    return std::nullopt;
}
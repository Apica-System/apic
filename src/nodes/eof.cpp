#include "nodes/eof.hpp"
#include <iostream>

using namespace nodes;

NodeEndOfFile::NodeEndOfFile(const utils::Position &position)
    : Node(position) {

}

void NodeEndOfFile::show(std::string &indent, char end) const {
    std::cout << indent << "NodeEndOfFile" << end;
}

NodeKind NodeEndOfFile::getKind() const {
    return NodeKind::EndOfFile;
}

void NodeEndOfFile::emit(core::Emitter &) const {
    
}

void NodeEndOfFile::setId() {
    
}

std::optional<nodes::Node*> NodeEndOfFile::optimize(core::Optimizer &) {
    return std::nullopt;
}
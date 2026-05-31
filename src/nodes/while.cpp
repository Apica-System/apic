#include "nodes/while.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeWhile::NodeWhile(const utils::Position &position, Node *cnd, Node *body)
    : Node(position), condition(cnd), body(body) {

}

NodeWhile::~NodeWhile() {
    delete this->condition;
    delete this->body;
}

void NodeWhile::show(std::string &indent, char end) const {
    std::cout << indent << "NodeWhile" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->condition->show(indent, end);
    this->body->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeWhile::getKind() const {
    return NodeKind::While;
}

void NodeWhile::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::While);    
    this->condition->emit(emitter);    
    this->body->emit(emitter);
}

void NodeWhile::setId() {
    this->condition->setId();
    this->body->setId();
}

std::optional<nodes::Node*> NodeWhile::optimize(core::Optimizer &) {
    return std::nullopt;
}

Node *NodeWhile::getCondition() const {
    return this->condition;
}

Node *NodeWhile::getBody() const {
    return this->body;
}
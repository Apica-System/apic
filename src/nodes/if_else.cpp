#include "nodes/if_else.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeIfElse::NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body)
    : Node(position), if_condition(if_cnd), if_body(if_body), else_body(std::nullopt) {

}

NodeIfElse::NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body, Node *else_body)
    : Node(position), if_condition(if_cnd), if_body(if_body), else_body(else_body) {

}

NodeIfElse::~NodeIfElse() {
    delete this->if_condition;
    delete this->if_body;
    if (this->else_body) delete this->else_body.value();
}

void NodeIfElse::show(std::string &indent, char end) const {
    std::cout << indent << "NodeIfElse(else: " << this->else_body.has_value() << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->if_condition->show(indent, end);
    this->if_body->show(indent, end);
    if (this->else_body) this->else_body.value()->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeIfElse::getKind() const {
    return NodeKind::IfElse;
}

void NodeIfElse::emit(core::Emitter &emitter) const {
    if (this->else_body) {
        emitter.writeU64(common::bytecodes::ApicaBytecode::IfElse);

        this->if_condition->emit(emitter);
        this->if_body->emit(emitter);
        this->else_body.value()->emit(emitter);
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::If);

        this->if_condition->emit(emitter);
        this->if_body->emit(emitter);
    }
}

void NodeIfElse::setId() {
    this->if_condition->setId();
    this->if_body->setId();
    if (this->else_body) this->else_body.value()->setId();
}

std::optional<nodes::Node*> NodeIfElse::optimize(core::Optimizer &) {
    return std::nullopt;
}

Node *NodeIfElse::getIfCondition() const {
    return this->if_condition;
}

Node *NodeIfElse::getIfBody() const {
    return this->if_body;
}

std::optional<Node*> NodeIfElse::getElseBody() const {
    return this->else_body;
}
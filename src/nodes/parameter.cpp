#include "nodes/parameter.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include <iostream>

using namespace nodes;

NodeParameter::NodeParameter(const utils::Position &position, Node *expression)
    : Node(position), expression(expression), parameter_name(std::nullopt) {

}

NodeParameter::NodeParameter(const utils::Position &position, Node *expression, const std::string &name)
    : Node(position), expression(expression), parameter_name(name) {

}

NodeParameter::~NodeParameter() {
    delete this->expression;
}

void NodeParameter::show(std::string &indent, char end) const {
    std::cout << indent << "NodeParameter(name: " << this->parameter_name.value_or("?") << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->expression->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeParameter::getKind() const {
    return NodeKind::Parameter;
}

void NodeParameter::emit(core::Emitter &emitter) const {
    this->expression->emit(emitter);
}

void NodeParameter::setId() {
    this->expression->setId();
}

std::optional<nodes::Node*> NodeParameter::optimize(core::Optimizer &optimizer) {
    std::optional<nodes::Node*> optimized = this->expression->optimize(optimizer);
    if (optimized)
        optimizer.swapNode(this->expression, optimized.value());

    return std::nullopt;
}
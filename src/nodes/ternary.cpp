#include "nodes/ternary.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeTernaryOperation::NodeTernaryOperation(const utils::Position &position, Node *cnd, Node *true_expr, Node *false_expr)
    : Node(position), condition(cnd), true_expr(true_expr), false_expr(false_expr) {

}

NodeTernaryOperation::~NodeTernaryOperation() {
    delete this->condition;
    delete this->false_expr;
    delete this->true_expr;
}

void NodeTernaryOperation::show(std::string &indent, char end) const {
    std::cout << indent << "NodeTernaryOperation" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->condition->show(indent, end);
    this->true_expr->show(indent, end);
    this->false_expr->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeTernaryOperation::getKind() const {
    return NodeKind::TernaryOperation;
}

void NodeTernaryOperation::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::QuestionOperation);

    this->condition->emit(emitter);
    this->true_expr->emit(emitter);
    this->false_expr->emit(emitter);
}

void NodeTernaryOperation::setId() {
    this->condition->setId();
    this->true_expr->setId();
    this->false_expr->setId();
}

std::optional<nodes::Node*> NodeTernaryOperation::optimize(core::Optimizer &) {
    return std::nullopt;
}
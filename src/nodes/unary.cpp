#include "nodes/unary.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include <iostream>

using namespace nodes;

NodeUnaryOperation::NodeUnaryOperation(const utils::Position &position, utils::TokenKind op, Node *operand)
    : Node(position), unary_operator(op), operand(operand) {

}

NodeUnaryOperation::~NodeUnaryOperation() {
    delete this->operand;
}

void NodeUnaryOperation::show(std::string &indent, char end) const {
    std::cout << indent << "NodeUnaryOperator(op: " << static_cast<uint16_t>(this->unary_operator) << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->operand->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeUnaryOperation::getKind() const {
    return NodeKind::UnaryOperation;
}

void NodeUnaryOperation::emit(core::Emitter &emitter) const {
    switch (this->unary_operator) {
        case utils::TokenKind::Bang:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Not);
            break;

        default: {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_INCORRECT_UNARY_OPERATOR),
                this->position
            ));
        } break;
    }

    this->operand->emit(emitter);
}

void NodeUnaryOperation::setId() {
    this->operand->setId();
}

std::optional<nodes::Node*> NodeUnaryOperation::optimize(core::Optimizer &) {
    return std::nullopt;
}
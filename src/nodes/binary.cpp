#include "nodes/binary.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include <iostream>

using namespace nodes;

NodeBinaryOperation::NodeBinaryOperation(const utils::Position &position, utils::TokenKind op, Node *left) 
    : Node(position), binary_operator(op), left(left), right(std::nullopt) {

}

NodeBinaryOperation::NodeBinaryOperation(const utils::Position &position, utils::TokenKind op, Node *left, Node *right)
    : Node(position), binary_operator(op), left(left), right(right) {

}

NodeBinaryOperation::~NodeBinaryOperation() {
    delete this->left;
    if (this->right) delete this->right.value();
}

void NodeBinaryOperation::show(std::string &indent, char end) const {
    std::cout << indent << "NodeBinary(op: " << static_cast<uint16_t>(this->binary_operator) << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->left->show(indent, end);
    if (this->right) this->right.value()->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeBinaryOperation::getKind() const {
    return NodeKind::BinaryOperation;
}

void NodeBinaryOperation::emit(core::Emitter &emitter) const {
    switch (this->binary_operator) {
        case utils::TokenKind::Plus:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Add);
            break;
        
        case utils::TokenKind::PlusPlus:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Increment);
            break;
        
        case utils::TokenKind::Minus:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Subtract);
            break;
        
        case utils::TokenKind::MinusMinus:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Decrement);
            break;
        
        case utils::TokenKind::Equals:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Assign);
            break;
        
        case utils::TokenKind::EqualsEquals:
            emitter.writeU64(common::bytecodes::ApicaBytecode::Equals);
            break;
        
        case utils::TokenKind::Less:
            emitter.writeU64(common::bytecodes::ApicaBytecode::LessThan);
            break;
        
        case utils::TokenKind::As:
            emitter.writeU64(common::bytecodes::ApicaBytecode::As);
            break;

        default: {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_INCORRECT_BINARY_OPERATOR),
                this->position
            ));
        }
    }

    this->left->emit(emitter);
    if (this->right)
        this->right.value()->emit(emitter);
}

void NodeBinaryOperation::setId() {
    this->left->setId();
    if (this->right) this->right.value()->setId();
}

std::optional<nodes::Node*> NodeBinaryOperation::optimize(core::Optimizer &) {
    return std::nullopt;
}

utils::TokenKind NodeBinaryOperation::getBinaryOperator() const {
    return this->binary_operator;
}

Node *NodeBinaryOperation::getLeftOperand() const {
    return this->left;
}

std::optional<Node*> NodeBinaryOperation::getRightOperand() const {
    return this->right;
}
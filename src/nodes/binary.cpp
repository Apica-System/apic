#include "nodes/binary.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include <iostream>
#include "core/optimizer.hpp"
#include "elements.hpp"
#include "values/error.hpp"

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
        
        case utils::TokenKind::LessEquals:
            emitter.writeU64(common::bytecodes::ApicaBytecode::LessOrEquals);
            break;
        
        case utils::TokenKind::Greater:
            emitter.writeU64(common::bytecodes::ApicaBytecode::GreaterThan);
            break;
        
        case utils::TokenKind::GreaterEquals:
            emitter.writeU64(common::bytecodes::ApicaBytecode::GreaterOrEquals);
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

std::optional<nodes::Node*> NodeBinaryOperation::optimize(core::Optimizer &optimizer) {
    std::optional<nodes::Node*> optimized_left = this->left->optimize(optimizer);
    if (optimized_left)
        optimizer.swapNode(this->left, optimized_left.value());
    
    if (this->right) {
        std::optional<nodes::Node*> optimized_right = this->right.value()->optimize(optimizer);
        if (optimized_right)
            optimizer.swapNode(this->right.value(), optimized_right.value());
    }

    switch (this->binary_operator) {
        case utils::TokenKind::Plus: case utils::TokenKind::Minus:
            return this->optimizeFullBinary();

        default: return std::nullopt;
    }
}

std::optional<nodes::Node*> NodeBinaryOperation::optimizeFullBinary() {
    if (this->left->getKind() == NodeKind::Literal && this->right.value()->getKind() == NodeKind::Literal) {
        NodeLiteral *literal_left = static_cast<NodeLiteral*>(this->left);
        NodeLiteral *literal_right = static_cast<NodeLiteral*>(this->right.value());

        common::elements::Element element_left(
            common::elements::ElementModifier::Copy,
            literal_left->getValue()
        );

        common::elements::Element element_right(
            common::elements::ElementModifier::Copy,
            literal_right->getValue()
        );

        common::elements::Element *result = nullptr;
        switch (this->binary_operator) {
            case utils::TokenKind::Plus:
                result = element_left.add(&element_right);
                break;
            
            case utils::TokenKind::Minus:
                result = element_left.subtract(&element_right);
                break;
            
            default: break;
        }

        if (!result) return std::nullopt;

        if (result->isErrorOrController()) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                static_cast<common::values::ValueError*>(result->getValue()),
                this->position
            ));

            delete result;
        } else {
            result->addModifier(common::elements::ElementModifier::Copy);
            NodeLiteral *returned = new NodeLiteral(result->getValue());
            delete result;
            
            return returned;
        }
    }

    return std::nullopt;
}
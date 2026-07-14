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

utils::OptimizedResult NodeBinaryOperation::optimize(core::Optimizer &optimizer) {
    utils::OptimizedResult optimized_left = this->left->optimize(optimizer);
    if (optimized_left.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_NULL_OPERAND_FOUND),
            this->left->getPosition()
        ));
    } else if (optimized_left.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_left.swapWith(this->left);
    }
    
    if (this->right) {
        utils::OptimizedResult optimized_right = this->right.value()->optimize(optimizer);
        if (optimized_left.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::OPM_ERROR_NULL_OPERAND_FOUND),
                this->right.value()->getPosition()
            ));
        } else if (optimized_right.hasFlag(utils::OptimizedFlag::Optimized)) {
            optimized_right.swapWith(this->right.value());
        }
    }

    switch (this->binary_operator) {
        case utils::TokenKind::PlusPlus: case utils::TokenKind::MinusMinus:
            this->checkCorrectVarConstAccess(optimized_left);
            break;

        case utils::TokenKind::Plus: case utils::TokenKind::Minus:
        case utils::TokenKind::Less: case utils::TokenKind::LessEquals:
        case utils::TokenKind::Greater: case utils::TokenKind::GreaterEquals:
            return this->optimizeOperation();

        default: break;
    }

    return utils::OptimizedResult(utils::OptimizedFlag::None);
}

utils::OptimizedResult NodeBinaryOperation::optimizeOperation() {
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
            
            case utils::TokenKind::Less:
                result = element_left.lessThan(&element_right);
                break;
            
            case utils::TokenKind::LessEquals:
                result = element_left.lessOrEquals(&element_right);
                break;
            
            case utils::TokenKind::Greater:
                result = element_left.greaterThan(&element_right);
                break;
            
            case utils::TokenKind::GreaterEquals:
                result = element_left.greaterOrEquals(&element_right);
                break;
            
            default: break;
        }

        if (!result) 
            return utils::OptimizedResult(utils::OptimizedFlag::None);

        result->addModifier(common::elements::ElementModifier::Copy);
        if (result->isErrorOrController()) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                static_cast<common::values::ValueError*>(result->getValue()),
                this->position
            ));

            
            delete result;
        } else {
            common::values::Value *val = result->getValue();
            delete result;
            
            return utils::OptimizedResult(
                utils::OptimizedFlag::Literal | utils::OptimizedFlag::Optimized,
                new nodes::NodeLiteral(val)
            );
        }
    }

    return utils::OptimizedResult(utils::OptimizedFlag::None);
}

void NodeBinaryOperation::checkCorrectVarConstAccess(const utils::OptimizedResult &optimized_left) {
    if (!optimized_left.hasFlag(utils::OptimizedFlag::VarConstAccess)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_BINARY_OPERATOR_EXPECTED_VAR_CONST),
            this->position
        ));
    }
}
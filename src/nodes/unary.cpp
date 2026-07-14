#include "nodes/unary.hpp"
#include "nodes/literal.hpp"

#include "core/emitter.hpp"

#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"

#include "elements.hpp"

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

utils::OptimizedResult NodeUnaryOperation::optimize(core::Optimizer &optimizer) {
    utils::OptimizedResult optimized_operand = this->operand->optimize(optimizer);
    if (optimized_operand.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_NULL_OPERAND_FOUND),
            this->operand->getPosition()
        ));
    } else if (optimized_operand.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_operand.swapWith(this->operand);
    }

    switch (this->unary_operator) {
        case utils::TokenKind::PlusPlus: case utils::TokenKind::MinusMinus:
            this->checkCorrectVarConstAccess(optimized_operand);
            break;
        
        case utils::TokenKind::Bang: case utils::TokenKind::Tilde:
            return this->optimizeOperation();
        
        default: break;
    }

    return utils::OptimizedResult(utils::OptimizedFlag::None);
}

utils::OptimizedResult NodeUnaryOperation::optimizeOperation() {
    if (this->operand->getKind() == NodeKind::Literal) {
        NodeLiteral *literal = static_cast<NodeLiteral*>(this->operand);

        common::elements::Element element(
            common::elements::ElementModifier::Copy,
            literal->getValue()
        );

        common::elements::Element *result = nullptr;
        switch (this->unary_operator) {
            case utils::TokenKind::Bang:
                result = element.unaryNot();
                break;
            
            case utils::TokenKind::Tilde:
                result = element.bitwiseNot();
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

void NodeUnaryOperation::checkCorrectVarConstAccess(const utils::OptimizedResult &optimized_operand) {
    if (!optimized_operand.hasFlag(utils::OptimizedFlag::VarConstAccess)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_UNARY_OPERATOR_EXPECTED_VAR_CONST),
            this->position
        ));
    }
}
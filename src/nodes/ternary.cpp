#include "nodes/ternary.hpp"

#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/warnings.hpp"

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

utils::OptimizedResult NodeTernaryOperation::optimize(core::Optimizer &optimizer) {
    uint8_t useless_condition = 0;
    utils::OptimizedResult optimized_condition = this->condition->optimize(optimizer);
    if (optimized_condition.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_TERNARY_CONDITION_UNEXPECTED),
            this->condition->getPosition()
        ));
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Literal)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Warning,
            std::string(utils::OPM_WRN_USELESS_TERNARY),
            this->condition->getPosition()
        ));

        useless_condition = optimizer.getLiteralBooleanValue(optimized_condition)
            ? 1 : 2;
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_condition.swapWith(this->condition);
    }

    utils::OptimizedResult optimized_true_expr = this->true_expr->optimize(optimizer);
    if (optimized_true_expr.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_TERNARY_TRUE_UNEXPECTED),
            this->true_expr->getPosition()
        ));
    } else if (optimized_true_expr.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_true_expr.swapWith(this->true_expr);
    }

    utils::OptimizedResult optimized_false_expr = this->false_expr->optimize(optimizer);
    if (optimized_false_expr.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_TERNARY_FALSE_UNEXPECTED),
            this->false_expr->getPosition()
        ));
    } else if (optimized_false_expr.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_false_expr.swapWith(this->false_expr);
    }

    switch (useless_condition) {
        case 1: {
            nodes::Node *true_expr = this->true_expr;
            this->true_expr = nullptr;
            return utils::OptimizedResult(
                utils::OptimizedFlag::Optimized,
                true_expr
            );
        }

        case 2: {
            nodes::Node *false_expr = this->false_expr;
            this->false_expr = nullptr;
            return utils::OptimizedResult(
                utils::OptimizedFlag::Optimized,
                false_expr
            );
        }

        default: return utils::OptimizedResult(utils::OptimizedFlag::None);
    }
}
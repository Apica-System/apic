#include "nodes/return.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include <iostream>

using namespace nodes;

NodeReturn::NodeReturn(const utils::Position &position)
    : Node(position), returned_expression(std::nullopt) {

}

NodeReturn::NodeReturn(const utils::Position &position, Node *expr)
    : Node(position), returned_expression(expr) {

}

NodeReturn::~NodeReturn() {
    if (this->returned_expression) delete this->returned_expression.value();
}

void NodeReturn::show(std::string &indent, char end) const {
    std::cout << indent << "NodeReturn" << end;

    if (this->returned_expression) {
        indent.push_back(' ');
        indent.push_back(' ');
        this->returned_expression.value()->show(indent, end);

        indent.pop_back();
        indent.pop_back();
    }
}

NodeKind NodeReturn::getKind() const {
    return NodeKind::Return;
}

void NodeReturn::emit(core::Emitter &emitter) const {
    if (this->returned_expression) {
        emitter.writeU64(common::bytecodes::ApicaBytecode::FilledReturn);
        this->returned_expression.value()->emit(emitter);
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::BlankReturn);
    }
}

void NodeReturn::setId() {
    if (this->returned_expression) this->returned_expression.value()->setId();
}

utils::OptimizedResult NodeReturn::optimize(core::Optimizer &optimizer) {
    if (this->returned_expression) {
        utils::OptimizedResult optimized_returned = this->returned_expression.value()->optimize(optimizer);
        if (optimized_returned.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::OPM_ERROR_RETURN_UNEXPECTED),
                this->returned_expression.value()->getPosition()
            ));
        } else if (optimized_returned.hasFlag(utils::OptimizedFlag::Optimized) && !optimized_returned.hasFlag(utils::OptimizedFlag::Literal)) {
            optimized_returned.swapWith(this->returned_expression.value());
        }
    }

    return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
}
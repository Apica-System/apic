#include "nodes/while.hpp"
#include "nodes/eof.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/warnings.hpp"
#include "utils/errors.hpp"
#include <iostream>

using namespace nodes;

NodeWhile::NodeWhile(const utils::Position &position, Node *cnd, Node *body)
    : Node(position), condition(cnd), body(body) {

}

NodeWhile::~NodeWhile() {
    delete this->condition;
    delete this->body;
}

void NodeWhile::show(std::string &indent, char end) const {
    std::cout << indent << "NodeWhile" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->condition->show(indent, end);
    this->body->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeWhile::getKind() const {
    return NodeKind::While;
}

void NodeWhile::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::While);    
    this->condition->emit(emitter);    
    this->body->emit(emitter);
}

void NodeWhile::setId() {
    this->condition->setId();
    this->body->setId();
}

utils::OptimizedResult NodeWhile::optimize(core::Optimizer &optimizer) {
    bool is_useless = false;
    
    utils::OptimizedResult optimized_condition = this->condition->optimize(optimizer);
    if (optimized_condition.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_WHILE_UNEXPECTED_CONDITION),
            this->condition->getPosition()
        ));
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Literal)) {
        bool is_true = optimizer.getLiteralBooleanValue(optimized_condition);
        if (!is_true) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Warning,
                std::string(utils::OPM_WRN_USELESS_WHILE),
                this->condition->getPosition()
            ));

            is_useless = true;
        }
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_condition.swapWith(this->condition);
    }

    optimizer.addModifier(core::OptimizerModifier::AggregateCompound);
    utils::OptimizedResult optimized_body = this->body->optimize(optimizer);
    if (optimized_body.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_body.swapWith(this->body);
    } else if (optimized_body.hasFlag(utils::OptimizedFlag::Useless)) {
        utils::Position pos = this->body->getPosition();
        delete this->body;

        this->body = new nodes::NodeCompound(pos, {});
    }
    
    return is_useless
        ? utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull | utils::OptimizedFlag::Useless)
        : utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
}
#include "nodes/if_else.hpp"

#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "utils/diagnostic_bag.hpp"
#include "utils/warnings.hpp"
#include "utils/errors.hpp"

#include <iostream>

using namespace nodes;

NodeIfElse::NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body)
    : Node(position), if_condition(if_cnd), if_body(if_body), else_body(std::nullopt) {

}

NodeIfElse::NodeIfElse(const utils::Position &position, Node *if_cnd, Node *if_body, Node *else_body)
    : Node(position), if_condition(if_cnd), if_body(if_body), else_body(else_body) {

}

NodeIfElse::~NodeIfElse() {
    delete this->if_condition;
    delete this->if_body;
    if (this->else_body) delete this->else_body.value();
}

void NodeIfElse::show(std::string &indent, char end) const {
    std::cout << indent << "NodeIfElse(else: " << this->else_body.has_value() << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->if_condition->show(indent, end);
    this->if_body->show(indent, end);
    if (this->else_body) this->else_body.value()->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeIfElse::getKind() const {
    return NodeKind::IfElse;
}

void NodeIfElse::emit(core::Emitter &emitter) const {
    if (this->else_body) {
        emitter.writeU64(common::bytecodes::ApicaBytecode::IfElse);

        this->if_condition->emit(emitter);
        this->if_body->emit(emitter);
        this->else_body.value()->emit(emitter);
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::If);

        this->if_condition->emit(emitter);
        this->if_body->emit(emitter);
    }
}

void NodeIfElse::setId() {
    this->if_condition->setId();
    this->if_body->setId();
    if (this->else_body) this->else_body.value()->setId();
}

utils::OptimizedResult NodeIfElse::optimize(core::Optimizer &optimizer) {
    uint8_t is_useless = 0;
    
    utils::OptimizedResult optimized_if_condition = this->if_condition->optimize(optimizer);
    if (optimized_if_condition.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_IF_UNEXPECTED_CONDITION),
            this->if_condition->getPosition()
        ));
    } else if (optimized_if_condition.hasFlag(utils::OptimizedFlag::Literal)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Warning,
            std::string(utils::OPM_WRN_USELESS_IF),
            this->if_condition->getPosition()
        ));

        bool is_true = optimizer.getLiteralBooleanValue(optimized_if_condition);
        if (is_true) {
            is_useless = 1;
        } else {
            is_useless = this->else_body ? 2 : 3;
        }
    } else if (optimized_if_condition.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_if_condition.swapWith(this->if_condition);
    }

    utils::OptimizedResult optimized_if_body = this->if_body->optimize(optimizer);
    if (optimized_if_body.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_if_body.swapWith(this->if_body);
    } else if (optimized_if_body.hasFlag(utils::OptimizedFlag::Useless)) {
        utils::Position pos = this->if_body->getPosition();
        delete this->if_body;

        this->if_body = new nodes::NodeCompound(pos, {});
    }

    if (this->else_body) {
        utils::OptimizedResult optimized_else_body = this->else_body.value()->optimize(optimizer);
        if (optimized_else_body.hasFlag(utils::OptimizedFlag::Optimized)) {
            optimized_else_body.swapWith(this->else_body.value());
        } else if (optimized_else_body.hasFlag(utils::OptimizedFlag::Useless)) {
            utils::Position pos = this->else_body.value()->getPosition();
            delete this->else_body.value();

            this->else_body = new nodes::NodeCompound(pos, {});
        }
    }

    switch (is_useless) {
        case 1: {
            nodes::Node *if_body = this->if_body;
            this->if_body = nullptr;
            return utils::OptimizedResult(
                utils::OptimizedFlag::AlwaysNull | utils::OptimizedFlag::Optimized,
                if_body
            );
        }

        case 2: {
            nodes::Node *else_body = this->else_body.value();
            this->else_body = std::nullopt;
            return utils::OptimizedResult(
                utils::OptimizedFlag::AlwaysNull | utils::OptimizedFlag::Optimized,
                else_body
            );
        }

        case 3: return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull | utils::OptimizedFlag::Useless);
        default: return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
    }
}
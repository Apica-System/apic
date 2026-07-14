#include "nodes/for.hpp"
#include "nodes/literal.hpp"

#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "utils/id_generator.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/warnings.hpp"

#include "values/bool.hpp"

#include <iostream>

using namespace nodes;

NodeFor::NodeFor(const utils::Position &position,std::optional<Node*> init, Node *cnd, std::optional<Node*> end, Node *body)
    : Node(position), initialize(std::move(init)), condition(cnd), end(std::move(end)), body(body) {

}

NodeFor::~NodeFor() {
    if (this->initialize) delete this->initialize.value();
    delete this->condition;
    if (this->end) delete this->end.value();
    delete this->body;
}

void NodeFor::show(std::string &indent, char end) const {
    std::cout << indent << "NodeWhile" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    if (this->initialize) this->initialize.value()->show(indent, end);
    this->condition->show(indent, end);
    if (this->end) this->end.value()->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeFor::getKind() const {
    return NodeKind::For;
}

void NodeFor::emit(core::Emitter &emitter) const {
    if (this->initialize) 
        this->initialize.value()->emit(emitter);
    
    emitter.writeU64(common::bytecodes::ApicaBytecode::While);
    this->condition->emit(emitter);

    if (this->end) {
        emitter.writeU64(common::bytecodes::ApicaBytecode::Compound);
        emitter.writeU64(2);

        this->body->emit(emitter);
        this->end.value()->emit(emitter);

        emitter.writeU64(common::bytecodes::ApicaBytecode::EndOfBlock);
    } else {
        this->body->emit(emitter);
    }
}

void NodeFor::setId() {
    utils::IdGenerator::getInstance().pushContext();
    
    if (this->initialize) this->initialize.value()->setId();
    this->condition->setId();
    if (this->end) this->end.value()->setId();

    utils::IdGenerator::getInstance().popContext();
}

utils::OptimizedResult NodeFor::optimize(core::Optimizer &optimizer) {
    bool is_useless = false;

    if (this->initialize) {
        utils::OptimizedResult optimized_init = this->initialize.value()->optimize(optimizer);
        if (optimized_init.hasAnyFlag(utils::OptimizedFlag::Useless | utils::OptimizedFlag::Literal)) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Warning,
                std::string(utils::OPM_WRN_USELESS_FOR_INIT),
                this->initialize.value()->getPosition()
            ));
        } else if (optimized_init.hasFlag(utils::OptimizedFlag::Optimized)) {
            optimized_init.swapWith(this->initialize.value());
        }
    }

    utils::OptimizedResult optimized_condition = this->condition->optimize(optimizer);
    if (optimized_condition.hasFlag(utils::OptimizedFlag::AlwaysNull)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::OPM_ERROR_FOR_UNEXPECTED_CONDITION),
            this->condition->getPosition()
        ));
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Literal)) {
        bool is_true = optimizer.getLiteralBooleanValue(optimized_condition);
        if (!is_true) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Warning,
                std::string(utils::OPM_WRN_USELESS_FOR),
                this->condition->getPosition()
            ));

            is_useless = true;
        }
    } else if (optimized_condition.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized_condition.swapWith(this->condition);
    }

    if (this->end) {
        utils::OptimizedResult optimized_end = this->end.value()->optimize(optimizer);
        if (optimized_end.hasAnyFlag(utils::OptimizedFlag::Useless | utils::OptimizedFlag::Literal)) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Warning,
                std::string(utils::OPM_WRN_USELESS_FOR_END),
                this->end.value()->getPosition()
            ));
        } else if (optimized_end.hasFlag(utils::OptimizedFlag::Optimized)) {
            optimized_end.swapWith(this->end.value());
        }
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
#include "nodes/var_const_call.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/builtins.hpp"
#include "utils/id_generator.hpp"
#include <iostream>

using namespace nodes;

NodeVarConstCall::NodeVarConstCall(const utils::Position &position, const std::string &name)
    : Node(position), name(name) {

}

void NodeVarConstCall::show(std::string &indent, char end) const {
    std::cout << indent << "NodeVarConstCall(name: " << this->name << ')' << end;
}

NodeKind NodeVarConstCall::getKind() const {
    return NodeKind::VarConstCall;
}

void NodeVarConstCall::emit(core::Emitter &emitter) const {
    if (emitter.getModifier() & core::EmitterModifier::EM_Builtin) {
        auto builtin = utils::APICA_CONSTANTS.find(this->name);
        if (builtin == utils::APICA_CONSTANTS.end()) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_INCORRECT_VAR_CONST_BUILTIN),
                this->position
            ));
        } else {
            std::string indent;
            builtin->second.emit(emitter);
        }
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::VarConstCall);
        emitter.writeU64(this->id);
    }
}

void NodeVarConstCall::setId() {
    std::optional<uint64_t> associated_id = utils::IdGenerator::getInstance().getId(this->name);
    if (!associated_id) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_UNDEFINED_VC),
            this->position
        ));
        return;
    }

    this->id = associated_id.value();
}

std::optional<nodes::Node*> NodeVarConstCall::optimize(core::Optimizer &optimizer) {
    if (optimizer.getModifier() & core::OptimizerModifier::OM_Builtin) {
        auto builtin = utils::APICA_CONSTANTS.find(this->name);
        if (builtin != utils::APICA_CONSTANTS.end()) {
            return NodeLiteral::copyBuiltin(builtin->second);
        }
    }

    return std::nullopt;
}

std::string NodeVarConstCall::getVarConstName() const {
    return this->name;
}

uint64_t NodeVarConstCall::getDeclarationId() const {
    return this->id;
}
#include "nodes/func_call.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/builtins.hpp"
#include "utils/id_generator.hpp"
#include "core/optimizer.hpp"
#include <iostream>

using namespace nodes;

NodeFuncCall::NodeFuncCall(const utils::Position &position, const std::string &name, const std::vector<NodeParameter*> &params)
    : Node(position), function_name(name), parameters(params) {

}

NodeFuncCall::~NodeFuncCall() {
    for (NodeParameter *param : this->parameters)
        delete param;
}

void NodeFuncCall::show(std::string &indent, char end) const {
    std::cout << indent << "NodeFuncCall(name: " << this->function_name << ')' << end;

    if (!this->parameters.empty()) {
        indent.push_back(' ');
        indent.push_back(' ');
        for (NodeParameter *param : this->parameters)
            param->show(indent, end);

        indent.pop_back();
        indent.pop_back();
    }
}

NodeKind NodeFuncCall::getKind() const {
    return NodeKind::FuncCall;
}

void NodeFuncCall::emit(core::Emitter &emitter) const {
    if (emitter.getModifier() & core::EmitterModifier::EM_Builtin) {
        auto builtin_func = utils::APICA_BUILTIN_FUNCTIONS.find(this->function_name);
        if (builtin_func == utils::APICA_BUILTIN_FUNCTIONS.end()) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_UNKNOWN_FUNC_CALL),
                this->position
            ));
        } else {
            emitter.writeU64(common::bytecodes::ApicaBytecode::BuiltinFuncCall);
            emitter.writeU64(builtin_func->second.getBytecode());

            emitter.writeU64(this->parameters.size());
            emitter.removeModifier(core::EmitterModifier::EM_Builtin);
            for (nodes::NodeParameter *param : this->parameters) {
                param->emit(emitter);
            }
            emitter.addModifier(core::EmitterModifier::EM_Builtin);

            emitter.writeU64(common::bytecodes::ApicaBytecode::EndOfBlock);
        }
    } else {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::EMT_ERROR_UNKNOWN_FUNC_CALL),
            this->position
        ));
    }
}

void NodeFuncCall::setId() {
    utils::IdGenerator::getInstance().removeModifier(utils::IdGeneratorModifier::IGM_BUILTIN);
    for (nodes::Node *parameter : this->parameters)
        parameter->setId();
}

utils::OptimizedResult NodeFuncCall::optimize(core::Optimizer &optimizer) {
    optimizer.removeModifier(core::OptimizerModifier::Builtin);
    for (nodes::Node *node : this->parameters) {
        node->optimize(optimizer);
    }

    return utils::OptimizedResult(utils::OptimizedFlag::None);
}
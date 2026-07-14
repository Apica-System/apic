#include "nodes/package_call.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/builtins.hpp"
#include "utils/id_generator.hpp"
#include <iostream>

using namespace nodes;

NodePackageCall::NodePackageCall(const utils::Position &position, const std::string &name, Node *contained)
    : Node(position), package_name(name), contained(contained) {

}

NodePackageCall::~NodePackageCall() {
    delete this->contained;
}

void NodePackageCall::show(std::string &indent, char end) const {
    std::cout << indent << "NodePackageCall(name: " << this->package_name << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->contained->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodePackageCall::getKind() const {
    return NodeKind::PackageCall;
}

void NodePackageCall::emit(core::Emitter &emitter) const {
    if (this->package_name == utils::BUILTIN_PACKAGE) {
        emitter.addModifier(core::EmitterModifier::EM_Builtin);
        this->contained->emit(emitter);
        emitter.removeModifier(core::EmitterModifier::EM_Builtin);
    } else {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::EMT_ERROR_USER_DEFINED_PACKAGE)
        ));
    }
}

void NodePackageCall::setId() {
    if (this->package_name == "APICA") {
        utils::IdGenerator::getInstance().addModifier(utils::IdGeneratorModifier::IGM_BUILTIN);
        this->contained->setId();
        utils::IdGenerator::getInstance().removeModifier(utils::IdGeneratorModifier::IGM_BUILTIN);
    } else {
        this->contained->setId();
    }
}

utils::OptimizedResult NodePackageCall::optimize(core::Optimizer &optimizer) {
    optimizer.addModifier(core::OptimizerModifier::Builtin);
    utils::OptimizedResult optimized = this->contained->optimize(optimizer);

    return optimized;
}
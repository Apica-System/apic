#include "nodes/package_call.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/builtins.hpp"
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
    if (this->package_name == "APICA")
        return;

    this->contained->setId();
}

std::optional<nodes::Node*> NodePackageCall::optimize(core::Optimizer &optimizer) {
    optimizer.addModifier(core::OptimizerModifier::OM_Builtin);
    std::optional<nodes::Node*> optimized = this->contained->optimize(optimizer);
    optimizer.removeModifier(core::OptimizerModifier::OM_Builtin);

    return optimized;
}
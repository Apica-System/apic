#include "nodes/global_scope.hpp"
#include "nodes/eof.hpp"

#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "utils/id_generator.hpp"
#include "utils/warnings.hpp"
#include "utils/diagnostic_bag.hpp"

#include <iostream>

using namespace nodes;

NodeGlobalScope::NodeGlobalScope(const utils::Position &position, Node *contained)
    : Node(position), contained(contained) {

}

NodeGlobalScope::~NodeGlobalScope() {
    delete this->contained;
}

void NodeGlobalScope::show(std::string &indent, char end) const {
    std::cout << indent << "NodeGlobalScope" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->contained->show(indent, end);

    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeGlobalScope::getKind() const {
    return NodeKind::GlobalScope;
}

void NodeGlobalScope::emit(core::Emitter &emitter) const {
    emitter.addModifier(core::EmitterModifier::EM_UnpackCompound);
    this->contained->emit(emitter);
    emitter.removeModifier(core::EmitterModifier::EM_UnpackCompound);
}

void NodeGlobalScope::setId() {
    utils::IdGenerator::getInstance().addModifier(utils::IdGeneratorModifier::IGM_Global);
    this->contained->setId();
    utils::IdGenerator::getInstance().removeModifier(utils::IdGeneratorModifier::IGM_Global);
}

utils::OptimizedResult NodeGlobalScope::optimize(core::Optimizer &optimizer) {
    utils::OptimizedResult optimized = this->contained->optimize(optimizer);
    if (optimized.hasFlag(utils::OptimizedFlag::Literal)) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Warning,
            std::string(utils::OPM_WRN_USELESS_GLOBAL),
            this->position
        ));

        return utils::OptimizedResult::copy(optimized);
    } else if (optimized.hasFlag(utils::OptimizedFlag::Useless)) {
        return utils::OptimizedResult(utils::OptimizedFlag::Useless);
    } else if (optimized.hasFlag(utils::OptimizedFlag::Optimized)) {
        optimized.swapWith(this->contained);
    }

    return utils::OptimizedResult(optimized.hasFlag(utils::OptimizedFlag::AlwaysNull)
        ? utils::OptimizedFlag::AlwaysNull
        : utils::OptimizedFlag::None
    );
}
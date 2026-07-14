#include "nodes/compound.hpp"
#include "nodes/eof.hpp"

#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "utils/id_generator.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/warnings.hpp"

#include <iostream>

using namespace nodes;

NodeCompound::NodeCompound(const utils::Position &position, const std::vector<Node*> nodes)
    : Node(position), nodes(nodes) {

}

NodeCompound::~NodeCompound() {
    for (Node *node : this->nodes)
        delete node;
}

void NodeCompound::show(std::string &indent, char end) const {
    if (indent.empty())
        std::cout << "ROOT" << end;
    else
        std::cout << indent << "NodeCompound" << end;
    
    indent.push_back(' ');
    indent.push_back(' ');
    for (Node *node : this->nodes)
        node->show(indent, end);
    
    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeCompound::getKind() const {
    return NodeKind::Compound;
}

void NodeCompound::emit(core::Emitter &emitter) const {
    if (!(emitter.getModifier() & core::EmitterModifier::EM_UnpackCompound))
        emitter.writeU64(common::bytecodes::ApicaBytecode::Compound);

    emitter.writeU64(this->nodes.size());
    emitter.removeModifier(core::EmitterModifier::EM_UnpackCompound);
    for (Node *node : this->nodes)
        node->emit(emitter);

    emitter.writeU64(common::bytecodes::ApicaBytecode::EndOfBlock);
}

void NodeCompound::setId() {
    utils::IdGenerator::getInstance().pushContext();
    for (nodes::Node *node : this->nodes)
        node->setId();
    utils::IdGenerator::getInstance().popContext();
}

utils::OptimizedResult NodeCompound::optimize(core::Optimizer &optimizer) {
    bool aggregate_compound = optimizer.hasModifier(core::OptimizerModifier::AggregateCompound);
    optimizer.removeModifier(core::OptimizerModifier::AggregateCompound);

    for (auto it = this->nodes.begin(); it != this->nodes.end(); ) {
        nodes::Node *node = *it;

        utils::OptimizedResult optimized = node->optimize(optimizer);
        if (optimized.hasAnyFlag(utils::OptimizedFlag::Useless | utils::OptimizedFlag::Literal)) {
            delete node;

            it = this->nodes.erase(it);
            continue;
        } else if (optimized.hasFlag(utils::OptimizedFlag::Optimized)) {
            optimized.swapWith(node);
        }

        ++it;
    }

    if (this->nodes.empty()) {
        if (!aggregate_compound) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Warning,
                std::string(utils::OPM_WRN_USELESS_COMPOUND),
                this->position
            ));
        }

        return utils::OptimizedResult(utils::OptimizedFlag::Useless | utils::OptimizedFlag::AlwaysNull);
    }

    return utils::OptimizedResult(utils::OptimizedFlag::AlwaysNull);
}

const std::vector<Node*> &NodeCompound::getNodes() const {
    return this->nodes;
}

std::vector<Node*> &NodeCompound::getMutableNodes() {
    return this->nodes;
}

void NodeCompound::emitRoot(core::Emitter &emitter) const {
    for (Node *node : this->nodes)
        node->emit(emitter);
}
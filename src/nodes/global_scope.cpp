#include "nodes/global_scope.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/id_generator.hpp"
#include "nodes/eof.hpp"
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
    emitter.writeU64(common::bytecodes::ApicaBytecode::Global);
    
    emitter.addModifier(core::EmitterModifier::EM_UnpackCompound);
    this->contained->emit(emitter);
    emitter.removeModifier(core::EmitterModifier::EM_UnpackCompound);
    
    if (this->contained->getKind() != NodeKind::Compound)
        emitter.writeU64(common::bytecodes::ApicaBytecode::EndOfBlock);
}

void NodeGlobalScope::setId() {
    utils::IdGenerator::getInstance().setGlobal(true);
    this->contained->setId();
    utils::IdGenerator::getInstance().setGlobal(false);
}

std::optional<nodes::Node*> NodeGlobalScope::optimize(core::Optimizer &optimizer) {
    std::optional<nodes::Node*> optimized = this->contained->optimize(optimizer);
    if (optimized) {
        nodes::Node *opt_node = optimized.value();
        if (opt_node->getKind() == NodeKind::Literal) {
            optimizer.swapNode(this->contained, opt_node);
            return new NodeEndOfFile(utils::Position());
        } else if (opt_node->getKind() == NodeKind::EndOfFile) {
            return opt_node;
        }

        optimizer.swapNode(this->contained, opt_node);
    }

    return std::nullopt;
}
#include "nodes/entrypoint.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include <iostream>

using namespace nodes;

NodeEntrypoint::NodeEntrypoint(const utils::Position &position, common::bytecodes::ApicaEntrypointBytecode entry, Node *body)
    : Node(position), entry_bytecode(entry), body(body) {

}

NodeEntrypoint::~NodeEntrypoint() {
    delete this->body;
}

void NodeEntrypoint::show(std::string &indent, char end) const {
    std::cout << indent << "NodeEntrypoint(entry: " << this->entry_bytecode << ')' << end;

    indent.push_back(' ');
    indent.push_back(' ');
    this->body->show(indent, end);
    
    indent.pop_back();
    indent.pop_back();
}
NodeKind NodeEntrypoint::getKind() const {
    return NodeKind::Entrypoint;
}

void NodeEntrypoint::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::Entrypoint);
    emitter.writeU64(this->entry_bytecode);
    
    emitter.addModifier(core::EmitterModifier::EM_UnpackCompound);
    this->body->emit(emitter);
    emitter.removeModifier(core::EmitterModifier::EM_UnpackCompound);
}

void NodeEntrypoint::setId() {
    this->body->setId();
}

utils::OptimizedResult NodeEntrypoint::optimize(core::Optimizer &optimizer) {
    optimizer.addModifier(core::OptimizerModifier::AggregateCompound);
    this->body->optimize(optimizer);
    
    return utils::OptimizedResult(utils::OptimizedFlag::None);
}

common::bytecodes::ApicaEntrypointBytecode NodeEntrypoint::getEntryBytecode() const {
    return this->entry_bytecode;
}

Node *NodeEntrypoint::getBody() const {
    return this->body;
}
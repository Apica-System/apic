#include "nodes/compound.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"
#include "utils/id_generator.hpp"
#include "nodes/eof.hpp"
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

    emitter.addModifier(core::EmitterModifier::EM_UnpackCompound);
    emitter.writeU64(common::bytecodes::ApicaBytecode::EndOfBlock);
}

void NodeCompound::setId() {
    utils::IdGenerator::getInstance().pushContext();
    for (nodes::Node *node : this->nodes)
        node->setId();
    utils::IdGenerator::getInstance().popContext();
}

std::optional<nodes::Node*> NodeCompound::optimize(core::Optimizer &optimizer) {
    for (auto it = this->nodes.begin(); it != this->nodes.end(); ) {
        nodes::Node *node = *it;
        std::optional<nodes::Node*> optimized = node->optimize(optimizer);
        if (optimized) {
            if (optimized.value()->getKind() == NodeKind::EndOfFile
                || optimized.value()->getKind() == NodeKind::Literal) {
                optimizer.deleteNode(node, optimized.value());

                it = this->nodes.erase(it);
                continue;
            }

            optimizer.swapNode(node, optimized.value());
        }

        ++it;
    }

    if (this->nodes.empty())
        return new NodeEndOfFile(utils::Position());
    return std::nullopt;
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
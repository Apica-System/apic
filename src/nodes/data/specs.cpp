#include "nodes/data/specs.hpp"
#include "nodes/data/u64.hpp"
#include "core/emitter.hpp"
#include <iostream>

using namespace nodes;

NodeDataSpecifications::NodeDataSpecifications(const utils::Position &position, const std::vector<NodeData*> &data)
    : Node(position), data_nodes(data) {

}

NodeDataSpecifications::~NodeDataSpecifications() {
    for (NodeData *data_node : this->data_nodes)
        delete data_node;
}

void NodeDataSpecifications::show(std::string &indent, char end) const {
    std::cout << indent << "NodeSpecifications" << end;

    indent.push_back(' ');
    indent.push_back(' ');
    for (NodeData *data_node : this->data_nodes)
        data_node->show(indent, end);
    
    indent.pop_back();
    indent.pop_back();
}

NodeKind NodeDataSpecifications::getKind() const {
    return NodeKind::DataSpecs;
}

void NodeDataSpecifications::emit(core::Emitter &emitter) const {
    for (nodes::NodeData *data : this->data_nodes)
        data->emit(emitter);
    
    emitter.writeU64(common::bytecodes::ApicaSpecificationBytecode::EndOfSpecification);
}

void NodeDataSpecifications::setId() {
    
}

std::optional<nodes::Node*> NodeDataSpecifications::optimize(core::Optimizer &) {
    return std::nullopt;
}

const std::vector<NodeData*> &NodeDataSpecifications::getDataNodes() const {
    return this->data_nodes;
}

void NodeDataSpecifications::addIdCount(uint64_t count) {
    this->data_nodes.push_back(new NodeDataU64(
        utils::Position(0, 0, 0, 0),
        common::bytecodes::ApicaSpecificationBytecode::IdCount,
        count
    ));
}
#include "nodes/node.hpp"
using namespace nodes;

Node::Node(const utils::Position &position)
    : position(position) {

}

utils::Position Node::getPosition() const {
    return this->position;
}
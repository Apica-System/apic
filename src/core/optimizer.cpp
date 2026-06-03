#include "core/optimizer.hpp"
#include "utils/errors.hpp"
#include "nodes/eof.hpp"

using namespace core;

Optimizer::Optimizer()
    : modifier(OptimizerModifier::OM_None) {

}

uint8_t Optimizer::getModifier() const {
    return this->modifier;
}

void Optimizer::addModifier(OptimizerModifier modifier) {
    this->modifier |= modifier;
}

void Optimizer::removeModifier(OptimizerModifier modifier) {
    this->modifier &= ~modifier;
}

void Optimizer::deleteNode(nodes::Node *node, nodes::Node *optimized) {
    if (node != optimized) {
        delete node;
        delete optimized;
    } else {
        delete node;
    }
}

void Optimizer::swapNode(nodes::Node *&node, nodes::Node *optimized) {
    if (node != optimized) {
        delete node;
        node = optimized;
    }
}
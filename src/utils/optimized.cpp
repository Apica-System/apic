#include "utils/optimized.hpp"
#include "nodes/node.hpp"

using namespace utils;

OptimizedResult::OptimizedResult(uint8_t flags)
    : flags(flags), optimized(std::nullopt) {

}

OptimizedResult::OptimizedResult(uint8_t flags, nodes::Node *optimized)
    : flags(flags), optimized(optimized) {

}

OptimizedResult::~OptimizedResult() {
    if (this->optimized) delete this->optimized.value();
}

bool OptimizedResult::hasFlag(OptimizedFlag flag) const {
    return this->flags & flag;
}

bool OptimizedResult::hasAnyFlag(uint8_t flags) const {
    return this->flags & flags;
}

void OptimizedResult::swapWith(nodes::Node *&node) {
    if (!this->optimized)
        return;
    
    delete node;
    node = this->optimized.value();
    this->optimized = std::nullopt;
}

std::optional<nodes::Node*> OptimizedResult::getOptimized() const {
    return this->optimized;
}

OptimizedResult OptimizedResult::copy(OptimizedResult &from) {
    if (from.optimized) {
        nodes::Node* temp = from.optimized.value();
        from.optimized = std::nullopt;

        return OptimizedResult(from.flags, temp);
    } else {
        return OptimizedResult(from.flags);
    }
}
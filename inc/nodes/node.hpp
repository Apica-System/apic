#pragma once

#include "nodes/kind.hpp"
#include "utils/position.hpp"
#include <string>
#include <optional>

namespace core {
    class Emitter;
    class Optimizer;
}

namespace nodes {
    class Node {
    public:
        virtual ~Node() {}

        virtual void show(std::string &indent, char end) const = 0;
        virtual NodeKind getKind() const = 0;
        virtual void emit(core::Emitter &emitter) const = 0;
        virtual void setId() = 0;
        virtual std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) = 0;

        utils::Position getPosition() const;
    protected:
        utils::Position position;

        Node(const utils::Position &position);
    };
}
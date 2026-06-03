#pragma once

#include "nodes/compound.hpp"
#include "nodes/entrypoint.hpp"
#include "nodes/global_scope.hpp"
#include "nodes/literal.hpp"

#include <optional>

namespace core {
    enum OptimizerModifier : uint8_t {
        OM_None =           0b00000000,
        OM_Builtin =        0b00000001,
    };

    class Optimizer final {
    public:
        Optimizer();

        uint8_t getModifier() const;
        void addModifier(OptimizerModifier modifier);
        void removeModifier(OptimizerModifier modifier);

        void deleteNode(nodes::Node *node, nodes::Node *optimized);
        void swapNode(nodes::Node *&node, nodes::Node *optimized);
    private:
        uint8_t modifier;
    };
}
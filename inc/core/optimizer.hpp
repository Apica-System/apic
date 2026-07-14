#pragma once

#include "nodes/compound.hpp"
#include "nodes/entrypoint.hpp"
#include "nodes/global_scope.hpp"
#include "nodes/literal.hpp"

namespace core {
    enum OptimizerModifier : uint8_t {
        None =              0b00000000,
        Builtin =           0b00000001,
        AggregateCompound = 0b00000010
    };

    class Optimizer final {
    public:
        Optimizer();

        bool hasModifier(OptimizerModifier modifier) const;
        void addModifier(OptimizerModifier modifier);
        void removeModifier(OptimizerModifier modifier);

        bool getLiteralBooleanValue(utils::OptimizedResult &result);
    private:
        uint8_t modifier;
    };
}
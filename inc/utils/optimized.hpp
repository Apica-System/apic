#pragma once

#include <optional>
#include <cstdint>

namespace nodes {
    class Node;
}

namespace utils {
    enum OptimizedFlag : uint8_t {
        None =              0b00000000,
        AlwaysNull =        0b00000001,
        Useless =           0b00000010,
        Optimized =         0b00000100,
        Literal =           0b00001000,
        VarConstAccess =    0b00010000
    };

    class OptimizedResult {
    public:
        OptimizedResult(uint8_t flags);
        OptimizedResult(uint8_t flags, nodes::Node *optimized);
        ~OptimizedResult();

        bool hasFlag(OptimizedFlag flag) const;
        bool hasAnyFlag(uint8_t flags) const;
        void swapWith(nodes::Node *&node);
        std::optional<nodes::Node*> getOptimized() const;

        static OptimizedResult copy(OptimizedResult &from);
    private:
        uint8_t flags;
        std::optional<nodes::Node*> optimized;
    };
}
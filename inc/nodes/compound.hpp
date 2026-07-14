#pragma once

#include "nodes/node.hpp"
#include <vector>

namespace nodes {
    class NodeCompound final : public Node {
    public:
        NodeCompound(const utils::Position &position, const std::vector<Node*> nodes);
        ~NodeCompound();
    
        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;

        const std::vector<Node*> &getNodes() const;
        std::vector<Node*> &getMutableNodes();

        void emitRoot(core::Emitter &emitter) const;
    private:
        std::vector<Node*> nodes;
    };
}
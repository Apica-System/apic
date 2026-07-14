#pragma once

#include "nodes/data/data.hpp"
#include <vector>

namespace nodes {
    class NodeDataSpecifications final : public Node {
    public:
        NodeDataSpecifications(const utils::Position &position, const std::vector<NodeData*> &data);
        ~NodeDataSpecifications();

        void show(std::string &indent, char end) const override;
        NodeKind getKind() const override;
        void emit(core::Emitter &emitter) const override;
        void setId() override;
        utils::OptimizedResult optimize(core::Optimizer &optimizer) override;

        const std::vector<NodeData*> &getDataNodes() const;
        void addIdCount(uint64_t count);
    private:
        std::vector<NodeData*> data_nodes;
    };
}
#pragma once

#include "nodes/node.hpp"
#include "bytecodes.hpp"

namespace nodes {
    class NodeData : public Node {
    public:
        virtual ~NodeData() {}
    
        common::bytecodes::ApicaSpecificationBytecode getSpecificationBytecode() const;
        void setId() override;
        std::optional<nodes::Node*> optimize(core::Optimizer &optimizer) override;
    protected:
        NodeData(const utils::Position &position, common::bytecodes::ApicaSpecificationBytecode spec_bytecode);

        common::bytecodes::ApicaSpecificationBytecode spec_bytecode;
    };
}
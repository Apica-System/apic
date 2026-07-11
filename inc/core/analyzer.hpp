#pragma once

#include "nodes/compound.hpp"

namespace core {
    class Analyzer final {
    public:
        Analyzer();

        void analyze(nodes::NodeCompound *root, bool cmp);
    private:
        bool checkEntrypoints(nodes::NodeCompound *root);
        bool checkSpecifications(nodes::NodeCompound *root);

        void extractGlobals(nodes::NodeCompound *root);
        void extractSpecifications(nodes::NodeCompound *root);
        void setIds(nodes::NodeCompound *root);
    };
}
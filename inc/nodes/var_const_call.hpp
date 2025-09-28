#pragma once

#include "nodes/var_const_decl.hpp"

namespace nodes {
    class NodeVarConstCall : public Node {
    public:
        NodeVarConstCall(const apic_string &name);
        ~NodeVarConstCall();

        apic_string GetName() const;
    private:
        apic_string name;
    };
}
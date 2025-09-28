#include "nodes/var_const_call.hpp"

namespace nodes {
    NodeVarConstCall::NodeVarConstCall(const apic_string &name)
        : name(name) {

    }
    
    NodeVarConstCall::~NodeVarConstCall() {

    }

    apic_string NodeVarConstCall::GetName() const {
        return this->name;
    }
}
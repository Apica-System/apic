#include "nodes/var_const_decl.hpp"

namespace nodes {
    NodeVarConstDecl::NodeVarConstDecl(const apic_string &name, values::ValueKind vkind, Node *expr) 
        : name(name) {
        this->vkind = vkind;
        this->expression = expr;
    }
    
    NodeVarConstDecl::~NodeVarConstDecl() {
        if (this->expression) delete this->expression;
    }

    apic_string NodeVarConstDecl::GetName() const {
        return this->name;
    }
    
    const values::ValueKind NodeVarConstDecl::GetVKind() const {
        return this->vkind;
    }
    
    Node *NodeVarConstDecl::GetExpression() const {
        return this->expression;
    }
}
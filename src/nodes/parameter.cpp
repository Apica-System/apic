#include "nodes/parameter.hpp"

namespace nodes {
    NodeParameter::NodeParameter(apic_string *name, Node *expr) {
        this->explicit_name = name;
        this->expression = expr;
    }

    NodeParameter::NodeParameter(Node *expr) {
        this->explicit_name = nullptr;
        this->expression = expr;
    }
    
    NodeParameter::~NodeParameter() {
        if (this->expression) delete this->expression;
        if (this->explicit_name) delete this->explicit_name;
    }

    const NodeKind NodeParameter::GetKind() const {
        return NodeKind::ND_Parameter;
    }

    void NodeParameter::Show(apic_string indent) {
        if (this->explicit_name)
            printf("%sNodeParameter(name: %s)\n", indent.c_str(), this->explicit_name->c_str());
        else
            printf("%sNodeParameter\n", indent.c_str());
        
        indent.append("  ");
        this->expression->Show(indent);

        indent.pop_back();
        indent.pop_back();
    }
    
    const apic_string *NodeParameter::GetExplicitName() const {
        return this->explicit_name;
    }
    
    Node *NodeParameter::GetExpression() const {
        return this->expression;
    }
}
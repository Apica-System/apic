#include "nodes/var_const_decl.hpp"
#include "core/emitter.hpp"
#include "utils/builtins.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/id_generator.hpp"

#include <iostream>

using namespace nodes;

NodeVarConstDeclaration::NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, const std::string &vtype)
    : Node(position), name(name), is_constant(is_const), value_type(vtype), expression(std::nullopt) {

}

NodeVarConstDeclaration::NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, Node *expression)
    : Node(position), name(name), is_constant(is_const), value_type(std::nullopt), expression(expression) {

}

NodeVarConstDeclaration::NodeVarConstDeclaration(const utils::Position &position, const std::string &name, bool is_const, const std::string &vtype, Node *expression)
    : Node(position), name(name), is_constant(is_const), value_type(vtype), expression(expression) {

}

NodeVarConstDeclaration::~NodeVarConstDeclaration() {
    if (this->expression) delete this->expression.value();
}

void NodeVarConstDeclaration::show(std::string &indent, char end) const {
    std::cout << indent << "NodeVarConstDecl(name: " << this->name << ", const: " << this->is_constant << ", type: ";
    if (this->value_type)
        std::cout << this->value_type.value() << ')' << end;
    else
        std::cout << "any)" << end;

    if (this->expression) {
        indent.push_back(' ');
        indent.push_back(' ');
        this->expression.value()->show(indent, end);

        indent.pop_back();
        indent.pop_back();
    }
}

NodeKind NodeVarConstDeclaration::getKind() const {
    return NodeKind::VarConstDeclaration;
}

void NodeVarConstDeclaration::emit(core::Emitter &emitter) const {
    emitter.writeU64(this->is_constant ? common::bytecodes::ApicaBytecode::ConstDecl : common::bytecodes::ApicaBytecode::VarDecl);
    emitter.writeU64(this->id);
    
    if (this->value_type) {
        auto var_const_type = utils::APICA_TYPES.find(this->value_type.value());
        if (var_const_type == utils::APICA_TYPES.end()) {
            std::string error_message(utils::EMT_ERROR_VAR_CONST_INCORRECT_TYPE);
            error_message += this->value_type.value();
            error_message += '`';

            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                error_message
            ));
        } else {
            emitter.writeU64(var_const_type->second);
        }
    } else {
        emitter.writeU64(common::bytecodes::ApicaTypeBytecode::Any);
    }
    
    if (this->expression) {
        this->expression.value()->emit(emitter);
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::Literal);
        emitter.writeU64(common::bytecodes::ApicaTypeBytecode::Null);
    }
}

void NodeVarConstDeclaration::setId() {
    utils::IdGenerator::getInstance().setNewId(this->name, this->position);
    if (this->expression) this->expression.value()->setId();
}

std::optional<nodes::Node*> NodeVarConstDeclaration::optimize(core::Optimizer &) {
    return std::nullopt;
}
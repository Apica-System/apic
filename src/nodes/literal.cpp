#include "nodes/literal.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "values/u32.hpp"
#include "values/u64.hpp"
#include "values/bool.hpp"
#include "values/string.hpp"
#include <iostream>

using namespace nodes;

NodeLiteral::NodeLiteral(common::values::Value *value)
    : Node(utils::Position()), value(value), is_copy(false) {

}

NodeLiteral::NodeLiteral(const utils::Position &position, common::values::Value *value)
    : Node(position), value(value), is_copy(false) {

}

NodeLiteral::~NodeLiteral() {
    if (!this->is_copy) delete this->value;
}

NodeLiteral *NodeLiteral::copyBuiltin(const NodeLiteral &builtin) {
    NodeLiteral *literal = new NodeLiteral(builtin.getPosition(), builtin.getValue());
    literal->is_copy = true;
    return literal;
}

void NodeLiteral::show(std::string &indent, char end) const {
    std::cout << indent << "NodeLiteral(val: ";
    this->value->show('\0');
    std::cout << ')' << end;
}

NodeKind NodeLiteral::getKind() const {
    return NodeKind::Literal;
}

void NodeLiteral::emit(core::Emitter &emitter) const {
    emitter.writeU64(common::bytecodes::ApicaBytecode::Literal);
    switch (this->value->getKind()) {
        case common::bytecodes::ApicaTypeBytecode::Null:
            emitter.writeU64(common::bytecodes::ApicaTypeBytecode::Null);
            break;
        
        case common::bytecodes::ApicaTypeBytecode::U32: {
            common::values::ValueU32 *u32 = static_cast<common::values::ValueU32*>(this->value);
            emitter.writeU64(common::bytecodes::ApicaTypeBytecode::U32);
            emitter.writeU32(u32->getValue().value());
        } break;
        
        case common::bytecodes::ApicaTypeBytecode::U64:
            emitter.writeU64(common::bytecodes::ApicaTypeBytecode::U64);
            emitter.writeU64(static_cast<common::values::ValueU64*>(this->value)->getValue().value());
            break;
        
        case common::bytecodes::ApicaTypeBytecode::Bool:
            emitter.writeU64(common::bytecodes::ApicaTypeBytecode::Bool);
            emitter.writeU8(static_cast<common::values::ValueBool*>(this->value)->getValue().value());
            break;
        
        case common::bytecodes::ApicaTypeBytecode::String:
            emitter.writeU64(common::bytecodes::ApicaTypeBytecode::String);
            emitter.writeString(static_cast<common::values::ValueString*>(this->value)->getValue().value());
            break;

        default: {
            std::string error_message(utils::EMT_ERROR_INCORRECT_LITERAL);
            error_message += std::to_string(this->value->getKind());
            error_message += '`';

            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                error_message
            ));
        }
    }
}

void NodeLiteral::setId() {
    
}

std::optional<nodes::Node*> NodeLiteral::optimize(core::Optimizer &) {
    return this;
}

common::values::Value *NodeLiteral::getValue() const {
    return this->value;
}
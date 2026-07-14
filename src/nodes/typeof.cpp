#include "nodes/typeof.hpp"
#include "core/emitter.hpp"
#include "utils/builtins.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include <iostream>

using namespace nodes;

NodeTypeof::NodeTypeof(const utils::Position &position, const std::string &vtype)
    : Node(position), value_type(vtype) {

}

void NodeTypeof::show(std::string &indent, char end) const {
    std::cout << indent << "NodeTypeof(type: " << this->value_type << ')' << end;
}

NodeKind NodeTypeof::getKind() const {
    return NodeKind::Typeof;
}

void NodeTypeof::emit(core::Emitter &emitter) const {
    auto declared_type = utils::APICA_TYPES.find(this->value_type);
    if (declared_type == utils::APICA_TYPES.end()) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::EMT_ERROR_INCORRECT_TYPE),
            this->position
        ));
    } else {
        emitter.writeU64(common::bytecodes::ApicaBytecode::Literal);
        emitter.writeU64(common::bytecodes::ApicaTypeBytecode::Type);
        emitter.writeU64(declared_type->second);
    }
}

void NodeTypeof::setId() {
    
}

utils::OptimizedResult NodeTypeof::optimize(core::Optimizer &) {
    return utils::OptimizedResult(utils::OptimizedFlag::None);
}
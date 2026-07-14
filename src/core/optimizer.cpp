#include "core/optimizer.hpp"
#include "utils/errors.hpp"
#include "nodes/eof.hpp"
#include "values/bool.hpp"

using namespace core;

Optimizer::Optimizer()
    : modifier(OptimizerModifier::None) {

}

bool Optimizer::hasModifier(OptimizerModifier modifier) const {
    return this->modifier & modifier;
}

void Optimizer::addModifier(OptimizerModifier modifier) {
    this->modifier |= modifier;
}

void Optimizer::removeModifier(OptimizerModifier modifier) {
    this->modifier &= ~modifier;
}

bool Optimizer::getLiteralBooleanValue(utils::OptimizedResult &result) {
    nodes::NodeLiteral *literal = static_cast<nodes::NodeLiteral*>(result.getOptimized().value());
    common::values::ValueBool *boolean_value;
    
    std::optional<common::values::Value*> convertion_result = literal->getValue()->autoConvert(common::bytecodes::ApicaTypeBytecode::Bool);
    if (!convertion_result) {
        convertion_result = literal->getValue()->convert(common::bytecodes::ApicaTypeBytecode::Bool);
    }
    boolean_value = static_cast<common::values::ValueBool*>(convertion_result.value());

    return boolean_value->getValue().value_or(false);
}
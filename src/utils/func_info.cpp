#include "utils/func_info.hpp"

using namespace utils;

ParameterInfo::ParameterInfo(bool is_args)
    : is_args(is_args), default_value(std::nullopt) {

}

ParameterInfo::ParameterInfo(bool is_args, common::values::Value *default_value)
    : is_args(is_args), default_value(default_value) {

}

ParameterInfo::~ParameterInfo() {
    if (this->default_value) delete this->default_value.value();
}

bool ParameterInfo::isArgs() const {
    return this->is_args;
}

bool ParameterInfo::isRequired() const {
    return !this->default_value.has_value();
}

std::optional<common::values::Value*> ParameterInfo::getDefaultValue() const {
    return this->default_value;
}

BuiltinFunctionInfo::BuiltinFunctionInfo(common::bytecodes::ApicaBuiltinFunctionBytecode bytecode, const std::vector<ParameterInfo> &parameters) 
    : bytecode(bytecode), parameters(parameters) {

}

common::bytecodes::ApicaBuiltinFunctionBytecode BuiltinFunctionInfo::getBytecode() const {
    return this->bytecode;
}

const std::vector<ParameterInfo> &BuiltinFunctionInfo::getParameters() const {
    return this->parameters;
}
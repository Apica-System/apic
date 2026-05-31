#pragma once

#include <cstdint>
#include <vector>
#include "values/value.hpp"
#include "bytecodes.hpp"

namespace utils {
    class ParameterInfo {
    public:
        ParameterInfo(bool is_args);
        ParameterInfo(bool is_args, common::values::Value *default_value);
        ~ParameterInfo();

        bool isArgs() const;
        bool isRequired() const;
        std::optional<common::values::Value*> getDefaultValue() const;
    private:
        bool is_args;
        std::optional<common::values::Value*> default_value;
    };

    class BuiltinFunctionInfo {
    public:
        BuiltinFunctionInfo(common::bytecodes::ApicaBuiltinFunctionBytecode bytecode, const std::vector<ParameterInfo> &parameters);

        common::bytecodes::ApicaBuiltinFunctionBytecode getBytecode() const;
        const std::vector<ParameterInfo> &getParameters() const;
    private:
        common::bytecodes::ApicaBuiltinFunctionBytecode bytecode;
        std::vector<ParameterInfo> parameters;
    };
}
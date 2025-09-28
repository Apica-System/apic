#pragma once

#include "utils/diagnostic_bag.hpp"

namespace utils {
    extern std::unordered_map<apic_string, ApicaBuiltinFuncCallBytecode> builtin_funcs;
    void init_builtin_funcs();

    ApicaBuiltinFuncCallBytecode get_builtin_func_bytecode(const apic_string &func_name, DiagnosticBag *diag_bag, utils::Position *position);
}
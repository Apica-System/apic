#include "utils/bytecode.hpp"

namespace utils {
    std::unordered_map<apic_string, ApicaBuiltinFuncCallBytecode> builtin_funcs;

    void init_builtin_funcs() {
        builtin_funcs["Quit"] = ApicaBuiltinFuncCallBytecode::AFB_Quit;
        builtin_funcs["LognInfo"] = ApicaBuiltinFuncCallBytecode::AFB_LognInfo;
        builtin_funcs["LogSuccess"] = ApicaBuiltinFuncCallBytecode::AFB_LogSuccess;
        builtin_funcs["LognSuccess"] = ApicaBuiltinFuncCallBytecode::AFB_LognSuccess;
    }

    ApicaBuiltinFuncCallBytecode get_builtin_func_bytecode(const apic_string &func_name, DiagnosticBag *diag_bag, utils::Position *position) {
        if (builtin_funcs.empty())
            init_builtin_funcs();
        
        if (!builtin_funcs.count(func_name)) {
            diag_bag->Add(new Diagnostic(
                DiagnosticKind::DIAG_Error,
                "EmitterError: Incorrect builtin function name",
                new utils::Position(position)
            ));
            return ApicaBuiltinFuncCallBytecode::AFB_Quit;
        }

        return builtin_funcs.at(func_name);
    }
}
#pragma once

#include "core/options.hpp"

namespace utils {
    bool init_apic_workdir();
    bool build_bytecode_file(const core::ApicOptions &apic_options);
}
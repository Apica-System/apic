#pragma once

#include "utils/decoder.hpp"

namespace utils {
    void write_i8(FILE *file, apic_i8 word);
    void write_u8(FILE *file, apic_u8 word);
    void write_u64(FILE *file, apic_u64 word);
}
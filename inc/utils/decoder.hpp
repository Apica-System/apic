#pragma once

#include "utils/source.hpp"

namespace utils {
    apic_u32 decode_integer(apic_string text);
    apic_u32 decode_binary(apic_string text);
    apic_u32 decode_octal(apic_string text);
    apic_u32 decode_hexadecimal(apic_string text);
    void __decode_special_char(apic_string text, apic_u64 *index, apic_char *special);
    apic_string decode_string(apic_string text);
}
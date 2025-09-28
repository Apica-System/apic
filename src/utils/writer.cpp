#include "utils/writer.hpp"

namespace utils {
    void write_i8(FILE *file, apic_i8 word) {
        fputc(word, file);
    }

    void write_u8(FILE *file, apic_u8 word) {
        fputc(word, file);
    }

    void write_u64(FILE *file, apic_u64 word) {
        apic_u8 bytes[8];
        for (apic_u8 i = 0; i < 8; i++)
            bytes[i] = (apic_u8)((word >> (8 * i)) & 0xFF);

        fwrite(bytes, 1, 8, file);
    }
}
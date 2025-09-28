#include "utils/decoder.hpp"

namespace utils {
    apic_u32 decode_integer(apic_string text) {
        apic_u32 result = 0;
        for (apic_char c : text)
            result = result * 10 + (c - '0');
        
        return result;
    }

    apic_u32 decode_binary(apic_string text) {
        apic_u32 result = 0;
        for (apic_char c : text)
            result = result * 2 + (c - '0');
        
        return result;
    }

    apic_u32 decode_octal(apic_string text) {
        apic_u32 result = 0;
        for (apic_char c : text)
            result = result * 8 + (c - '0');
        
        return result;
    }

    apic_u32 decode_hexadecimal(apic_string text) {
        apic_u32 result = 0;
        for (apic_char c : text)
            result = result * 16 + hexa_char_value(c);

        return result;
    }

    void __decode_special_char(apic_string text, apic_u64 *index, apic_char *special) {
        *index += 2;
        apic_char start = text[*index - 1];
        for (apic_u8 i = 0; i < 4; i++)
            special[i] = 0;

        switch (start) {
            case 'n':
                special[0] = '\n';
                break;
            
            case 't':
                special[0] = '\t';
                break;

            default:
                special[0] = start;
                break;
        }
    }

    apic_string decode_string(apic_string text) {
        apic_string result;
        apic_u64 index = 0;
        apic_char special[4] = { 0 };

        while (index < text.length()) {
            apic_char current = text[index];
            if (current == '\\') {
                __decode_special_char(text, &index, special);
                result.append(special);
            } else
                result.push_back(current);

            index++;
        }

        return result;
    }
}
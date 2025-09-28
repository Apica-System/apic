#include "common/chars.hpp"

unsigned char chars_length_utf8(unsigned char c) {
    if (c < 0x80) return 1;
    if (c < 0xe0) return 2;
    if (c < 0xf0) return 3;
    return 4;
}

unsigned char char_is_id_part(unsigned char c) {
    if (c < 0x80) {
        if (c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            return 1;
        return 0;
    } else if (c < 0xe0)
        return 2;
    else if (c < 0xf0)
        return 3;
    
    return 4;
}

bool char_is_binary(char c) {
    return c == '0' || c == '1';
}

bool char_is_octal(char c) {
    return c >= '0' && c <= '7';
}

bool char_is_hexa(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

unsigned char hexa_char_value(char c) {
    if (c < 'A')
        return c - '0';
    else if (c < 'a')
        return c - 'A';
    else
        return c - 'a';
}